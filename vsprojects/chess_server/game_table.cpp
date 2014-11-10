#include "game_table.h"
#include "chess_server_proto.h"
#include "game_user.h"
#include "chess_server.h"

GameTable::GameTable() {
  game_logi_ = nullptr;
}


GameTable::~GameTable() {
  if (game_logi_) {
    server_->GetGameFactory()->DestroyGame(game_logi_);
  }
}


void GameTable::Init(ChessServer* server, int table_id, int seat_count) {
  server_ = server;
  table_id_ = table_id;
  users_.resize(seat_count);
  game_logi_ = server_->GetGameFactory()->CreateGame(this);
}


void GameTable::BroadcastMessage(const void *mess, uint16_t sz) {
  for (int i = 0; i < (int)users_.size(); ++i) {
    SendMessageTo(i, mess, sz);
  }
}


void GameTable::BroadcastMessageExcept(seat_id_t seat_id, const void *mess, uint16_t sz) {
  for (int i = 0; i < (int)users_.size(); ++i) {
    if (i == seat_id) { continue; }
    SendMessageTo(i, mess, sz);
  }
}


void GameTable::SendMessageTo(seat_id_t seat_id, const void *mess, uint16_t sz) {
  if (!sz) { return; }
  std::unique_ptr<char[]> buffer(new char[1 + sz]);
  buffer[0] = CMD_GAME;
  memcpy(&buffer[1], mess, sz);
  GameUserPtr user = users_[seat_id];
  if (user && user->session_) {
    user->session_->SendMessage(buffer.get(), 1 + sz);
  }
}


void GameTable::HandleMessage(GameUserPtr user, uint8_t type, const void* data, uint16_t sz) {
  assert(user->table_ == this);
  assert(GetSeat(user) != -1);
  switch (type) {
  case CMD_SITDOWN:
    HandleSitdown(user);
    break;
  case CMD_STANDUP:
    HandleStandup(user);
    break;
  case CMD_READY:
    HandleReady(user);
    break;
  case CMD_GAME:
    HandleGame(user, data, sz);
    break;
  }
}


void GameTable::HandleSitdown(GameUserPtr user) {
  user->user_state_ = USER_STATE_SITDOWN;
  CmdTableUserEnter tue;
  tue.user_id = user->user_id_;
  tue.table_id_ = table_id_;
  tue.seat_id_ = GetSeat(user);
  server_->SendMessageToAll(CMD_SITDOWN, &tue, sizeof(tue));
  if (game_logi_) { game_logi_->OnUserEnter(user.get()); }
}


void GameTable::HandleStandup(GameUserPtr user) {
  user->user_state_ = USER_STATE_STANDUP;
  if (game_logi_) { game_logi_->OnUserLeave(user.get()); }
  CmdUpdateUserState uus;
  uus.user_id = user->user_id_;
  uus.state = USER_STATE_STANDUP;
  server_->SendMessageToAll(CMD_UPDATEUSERSTATE, &uus, sizeof(uus));
  SetUser(GetSeat(user), nullptr);
}


void GameTable::HandleReady(GameUserPtr user) {
  user->user_state_ = USER_STATE_READY;
  CmdUpdateUserState uus;
  uus.user_id = user->user_id_;
  uus.state = USER_STATE_READY;
  server_->SendMessageToAll(CMD_UPDATEUSERSTATE, &uus, sizeof(uus));

  for (GameUserPtr user : users_) {
    if (!user) { return; } // 人数不满
    if (user->user_state_ != USER_STATE_READY) { return; } // 玩家未准备
  }
  // TODO: state -> play
  bool broadcast = game_logi_->OnGameStart();
  if (broadcast) {
    for (int pos = 0; pos < (int)users_.size(); ++pos) {
      if (users_[pos]) { SendMessageTo(pos); }
    }
  }
}


void GameTable::HandleGame(GameUserPtr user, const void* data, uint16_t sz) {
  game_logi_->HandleMessage(user.get(), data, sz);
}
