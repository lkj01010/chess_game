#include "GameUser.h"
#include "GameTable.h"

GameUser::GameUser() {}


GameUser::~GameUser() {
}

table_id_t GameUser::table_id() const {
  if (on_table_) { return on_table_->table_id(); }
  return -1;
}

seat_id_t GameUser::seat_id() {
  if (on_table_) { return on_table_->GetSeat(shared_from_this()); }
  return -1;
}
