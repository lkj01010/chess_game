#ifndef GAMEUSER_H
#define GAMEUSER_H

#include <cstdint>
#include <string>
#include <memory>
#include "chess_server_proto.h"


class GameTable;
class GameUser;
typedef std::shared_ptr<GameUser> GameUserPtr;


class GameUser : public std::enable_shared_from_this<GameUser> {
public:
  static GameUserPtr Create() {
    return std::make_shared<GameUser>();
  }
  GameUser();
  ~GameUser();

  uint32_t user_id() const { return user_id_; }
  void set_user_id(uint32_t val) { user_id_ = val; }
  uint8_t user_state() const { return user_state_; }
  void set_user_state(uint8_t val) { user_state_ = val; }
  table_id_t table_id() const;
  seat_id_t seat_id();
  void set_on_table(GameTable* table) { on_table_ = table; }
  std::string nick_name() const { return nick_name_; }
  void set_nick_name(const std::string& val) { nick_name_ = val; }
  GameTable* on_table() const { return on_table_; }

private:
  uint32_t user_id_ = -1;
  uint8_t user_state_ = USER_STATE_STANDUP;
  uint32_t table_id_ = -1;
  std::string nick_name_;
  GameTable* on_table_ = nullptr;
};

#endif /* GAMEUSER_H */
