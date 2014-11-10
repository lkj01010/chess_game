#include "game_user.h"
#include "session.h"
#include "chess_server_proto.h"
#include "game_table.h"

static uint32_t uid = 0;

GameUser::GameUser() {
  user_id_ = ++uid;
  user_state_ = USER_STATE_STANDUP;
  table_ = nullptr;
}


GameUser::~GameUser() {
}


seat_id_t GameUser::seat_id() {
  if (table_) { return table_->GetSeat(shared_from_this()); }
  return -1;
}
