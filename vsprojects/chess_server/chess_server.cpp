#include <memory.h>
#include "chess_server.h"
#include "game_table.h"
#include "game_user.h"
#include "session.h"
#include "logger.h"


ChessServer::ChessServer(io_service &ios) : ios_(ios), gate_(ios) {
  game_factory_.reset(new GameFactory("landlords_moduled.dll"));
  for (int i = 0; i < DEFAULT_TABLE_COUNT; ++i) {
    tables_[i].Init(this, i, 3);
  }
}


ChessServer::~ChessServer() {
  unauth_sessions_.clear();
  RemoveUser();
}


int ChessServer::Start() {
  gate_.set_on_conn(std::bind(&ChessServer::OnConnection, this, std::placeholders::_1));
  if (gate_.Start(5678)) {
    return -1;
  }
  return 0;
}


void ChessServer::Stop() {
  ios_.stop();
}


void ChessServer::OnConnection(tcp::socket &socket) {
  SessionPtr session = Session::Create(socket);
  unauth_sessions_.insert(session);
  SessionWeakPtr wsession(session);
  session->SetMessageCallback([this, wsession](const void* data, uint16_t sz) {
    OnMessage(wsession, data, sz);
  });
  session->SetTimeoutCallback([this, wsession]() { OnTimeout(wsession); });
  session->SetCloseCallback([this, wsession]() { OnClose(wsession); });
}


void ChessServer::OnMessage(GameUserPtr user, const void* data, uint16_t sz) {
  CS_LOG_DEBUG("ChessServer::OnMessage");
  const uint8_t type = *(const uint8_t*)data;
  const void* message = (const char*)data + 1;
  switch (type) {
  case CMD_SITDOWN:
    HandleSitdown(user, (const CmdSitDown*)message);
    break;
  case CMD_RANDSITDOWN:
    HandleRandSitdown(user);
    break;
  case CMD_READY:
  case CMD_STANDUP:
  case CMD_GAME:
    ForwardTableMessage(user, type, message, sz - 1);
    break;
  case CMD_STOPSERVER:
    Stop();
    break;
  default:
    CS_LOG_DEBUG("unknown message type = " << type);
    break;
  }
}


void ChessServer::OnTimeout(GameUserPtr user) {
}


void ChessServer::OnClose(GameUserPtr user) {
  CS_LOG_DEBUG("ChessServer::OnClose");
  if (user->table_) {
    ForwardTableMessage(user, CMD_STANDUP, nullptr, 0);
  }
  RemoveUser(user);
}


void ChessServer::HandleSitdown(GameUserPtr user, const CmdSitDown *sitdown) {
  if (sitdown->table_id_ >= DEFAULT_TABLE_COUNT || sitdown->seat_id_ >= SEAT_COUNT) {
    return; // 非法参数
  }
  if (user->table_) { return; } // 该玩家已经坐下了
  GameTable& table = tables_[sitdown->table_id_];
  if (!table.IsEmptySeat(sitdown->seat_id_)) { return; } // 座位被别人坐了
  table.SetUser(sitdown->seat_id_, user);
  user->table_ = &table;
  ForwardTableMessage(user, CMD_SITDOWN, nullptr, 0);
}


void ChessServer::HandleRandSitdown(GameUserPtr user) {
  for (int t = 0; t < DEFAULT_TABLE_COUNT; ++t) {
    for (int s = 0; s < SEAT_COUNT; ++s) {
      if (tables_[t].IsEmptySeat(s)) {
        CmdSitDown seatdown;
        seatdown.table_id_ = t;
        seatdown.seat_id_ = s;
        HandleSitdown(user, &seatdown);
        break;
      }
    }
  }
}


void ChessServer::OnTimeout(SessionWeakPtr wsession) {
}


void ChessServer::OnClose(SessionWeakPtr wsession) {
  SessionPtr session = wsession.lock();
  if (!session) { return; }
  unauth_sessions_.erase(session);
}


void ChessServer::OnMessage(SessionWeakPtr wsession, const void* data, uint16_t sz) {
  SessionPtr session = wsession.lock();
  if (!session) { return; }
  const uint8_t type = *(const uint8_t*)data;
  const void* message = (const char*)data + 1;
  if (type == CMD_LOGIN) {
    HandleLogin(session, (const CmdLogin*)message);
  } else {
    CS_LOG_DEBUG("invalid message type");
    unauth_sessions_.erase(session);
  }
}


void ChessServer::HandleLogin(SessionPtr session, const CmdLogin* login) {
  GameUserPtr user = GameUser::Create();
  user->session_ = session;
  user->nick_name_ = login->uid;
  users_.insert(user);
  unauth_sessions_.erase(session);
  session->SetMessageCallback([this, user](const void* data, uint16_t sz) { OnMessage(user, data, sz); });
  session->SetTimeoutCallback([this, user]() { OnTimeout(user); });
  session->SetCloseCallback([this, user]() { OnClose(user); });

  size_t msize = sizeof(CmdLoginResponse) + sizeof(UserInfo) * (users_.size() - 1);
  CmdLoginResponse *response = (CmdLoginResponse*)malloc(msize);
  memset(response, 0, msize);
  response->succeed = 1;
  response->user_id = user->user_id_;
  response->sz = users_.size();
  int index = 0;
  for (GameUserPtr user : users_) {
    strcpy(response->users[index].nick_name, user->nick_name_.c_str());
    response->users[index].user_id = user->user_id_;
    response->users[index].state = user->user_state_;
    if (user->table_) {
      response->users[index].table_id = user->table_->table_id();
      response->users[index].seat_id = user->table_->GetSeat(user);
    } else {
      response->users[index].table_id = -1;
      response->users[index].seat_id = -1;
    }
    ++index;
  }
  SendMessageTo(user, CMD_LOGIN, response, msize);
  free(response);

  CmdAddUser adduser;
  strcpy(adduser.user.nick_name, user->nick_name_.c_str());
  adduser.user.state = user->user_state_;
  adduser.user.user_id = user->user_id_;
  SendMessageExcept(user, CMD_ADDUSER, &adduser, sizeof(adduser));
}


void ChessServer::SendMessageTo(GameUserPtr to, uint8_t type, const void* data, uint16_t sz) {
  if (!to || !to->session_) { return; }
  if (sz) {
    std::unique_ptr<char[]> buffer(new char[1 + sz]);
    buffer[0] = type;
    memcpy(&buffer[1], data, sz);
    to->session_->SendMessage(buffer.get(), 1 + sz);
  } else {
    to->session_->SendMessage(&type, 1);
  }
}


void ChessServer::SendMessageExcept(GameUserPtr except, uint8_t type, const void* data, uint16_t sz) {
  std::unique_ptr<char[]> buffer;
  if (sz) {
    buffer.reset(new char[1 + sz]);
    buffer[0] = type;
    memcpy(&buffer[1], data, sz);
  }
  for (GameUserPtr user : users_) {
    if (user == except) { continue; }
    if (!user->session_) { continue; }
    if (buffer) {
      user->session_->SendMessage(buffer.get(), 1 + sz);
    } else {
      user->session_->SendMessage(&type, 1);
    }
  }
}


void ChessServer::ForwardTableMessage(GameUserPtr user, uint8_t type, const void* data, uint16_t sz) {
  if (!user) { return; }
  if (!user->table_) { return; }
  if (-1 == user->table_->GetSeat(user)) { return; }
  user->table_->HandleMessage(user, type, data, sz);
}
