#include <Poco/ByteOrder.h>
#include "GameLogi.h"

using namespace boost::asio;

GameClient::GameClient() : socket_(ios_) {
  for (int i = 0; i < 100; ++i) {
    tables_[i].Init(i, 3);
  }
}


GameClient::~GameClient() {
  ios_.stop();
  if (thread_.joinable()) { thread_.join(); }
}


void GameClient::Start(const char *host, const char *port) {
  tcp::resolver resolver(ios_);
  auto endpoint_iterator = resolver.resolve({ host, port });
  boost::asio::async_connect(socket_, endpoint_iterator,
    std::bind(&GameClient::OnConnectCallback, this, std::placeholders::_1,
    std::placeholders::_2));

  thread_ = std::thread([&]() {
    while (!ios_.stopped()) {
      boost::system::error_code ec;
      ios_.run_one(ec);
    }
  });
}


void GameClient::Stop() {
  ios_.stop();
}


void GameClient::SendMessage(uint8_t type, const void* data, uint16_t sz) {
  uint16_t packet_size = 1 + sz;
  std::unique_ptr<char[]> packet(new char[packet_size]);
  *(uint8_t*)packet.get() = type;
  if (sz) { memcpy(&packet[1], data, sz); }
  session_->SendMessage(packet.get(), packet_size);
}


void GameClient::SendLogin(const std::string &uid, const std::string &passwd) {
  CmdLogin login;
  memset(&login, 0, sizeof(login));
  memcpy(login.uid, uid.c_str(), uid.size());
  memcpy(login.passwd, passwd.c_str(), passwd.size());
  SendMessage(CMD_LOGIN, &login, sizeof(login));
}


void GameClient::SendSeatDown(int table, int seat) {
  // TODO:
}


void GameClient::SendRandSeatDown() {
  SendMessage(CMD_RANDSITDOWN, nullptr, 0);
}


void GameClient::SendReady() {
  SendMessage(CMD_READY, nullptr, 0);
}


void GameClient::SendStandup() {
  SendMessage(CMD_STANDUP, nullptr, 0);
}


void GameClient::SendCommand(const void* data, uint16_t sz) {
  SendMessage(CMD_GAME, data, sz);
}


void GameClient::OnConnectCallback(error_code ec, tcp::resolver::iterator) {
  if (ec) {
    if (connection_monitor_) { connection_monitor_->OnDisconnected(false); }
    return;
  }
  if (connection_monitor_) { connection_monitor_->OnConnected(); }
  session_ = Session::Create(socket_);
  session_->SetMessageCallback(std::bind(&GameClient::HandleMessage, this,
    std::placeholders::_1, std::placeholders::_2));
}


void GameClient::HandleMessage(const void* data, uint16_t sz) {
  uint8_t type = *(uint8_t*)data;
  const void* message = (const char*)data + 1;
  switch (type) {
  case CMD_LOGIN:
    HandleLogin((CmdLoginResponse*)message);
    break;
  case CMD_ADDUSER:
    HandleAddUser((CmdAddUser*)message);
    break;
  case CMD_SITDOWN:
    HandleUserSitdown((CmdTableUserEnter*)message);
    break;
  case CMD_UPDATEUSERSTATE:
    HandleUpdateUserState((CmdUpdateUserState*)message);
    break;
  case CMD_GAME:
    HandleGame(message, sz - 1);
    break;
  default:
    break;
  }
}


void GameClient::HandleLogin(const CmdLoginResponse* response) {
  my_user_id_ = response->user_id;
  for (int i = 0; i < response->sz; ++i) {
    GameUserPtr user = GameUser::Create();
    user->set_nick_name(response->users[i].nick_name);
    user->set_user_id(response->users[i].user_id);
    user->set_user_state(response->users[i].state);
    user->set_on_table(response->users[i].table_id != 0xffff ? &tables_[response->users[i].table_id] : nullptr);
    users_[user->user_id()] = user;
    if (user->on_table()) {
      user->on_table()->SetSeatUser(response->users[i].seat_id, user);
    }
  }
  connection_monitor_->OnLogin();
}


void GameClient::HandleAddUser(const CmdAddUser* adduser) {
  GameUserPtr user = GameUser::Create();
  user->set_nick_name(adduser->user.nick_name);
  user->set_user_id(adduser->user.user_id);
  user->set_user_state(adduser->user.state);
  users_[user->user_id()] = user;
}


void GameClient::HandleUserSitdown(const CmdTableUserEnter* tue) {
  GameUserPtr user = GetUser(tue->user_id);
  tables_[tue->table_id_].SetSeatUser(tue->seat_id_, user);
  user->set_on_table(&tables_[tue->table_id_]);
  if (GetUser(my_user_id_)->table_id() == tue->table_id_) {
    game_logi_->OnUserEnter(user.get());
    if (tue->user_id == my_user_id_) {
      for (int i = 0; i < 3; ++i) {
        GameUserPtr tmp = tables_[tue->table_id_].GetUser(i);
        if (!tmp) { continue; }
        if (tmp != user) {
          game_logi_->OnUserEnter(tmp.get());
        }
      }
    }
  }
}


void GameClient::HandleUpdateUserState(const CmdUpdateUserState* uus) {
  GameUserPtr user = GetUser(uus->user_id);
  assert(user != nullptr);
  user->set_user_state(uus->state);
  if (!game_logi_) { return; }
  if (user->on_table() == GetMyself()->on_table()) {
    switch (user->user_state()) {
    case USER_STATE_STANDUP:
      game_logi_->OnUserLeave(user.get());
      break;
    case USER_STATE_READY:
      game_logi_->OnUserStateChange(user.get());
      break;
    }
  }
}


void GameClient::HandleGame(const void* data, uint16_t sz) {
  if (!game_logi_) { return; }
  game_logi_->HandleMessage(data, sz);
}
