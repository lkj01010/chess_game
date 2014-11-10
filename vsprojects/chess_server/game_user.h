#ifndef GAMEUSER_H
#define GAMEUSER_H

#include <cstdint>
#include <string>
#include <memory>
#include <boost/utility.hpp>
#include <chess_server/interfaces.h>
#include "session.h"

class GameTable;

class GameUser;
typedef std::shared_ptr<GameUser> GameUserPtr;
typedef std::weak_ptr<GameUser> GameUserWeakPtr;

class GameUser : public IUser, public boost::noncopyable, public std::enable_shared_from_this<GameUser> {
public:
  static GameUserPtr Create() {
    return std::make_shared<GameUser>();
  }

  GameUser();
  ~GameUser();

  const char* GetNickName() override {
    return nick_name_.c_str();
  }

  int32_t GetCoin() override { return 0; }

  void Kick() override {
    // TODO:
  }

  seat_id_t seat_id() override;
  
private:
  uint32_t user_id_;
  std::string nick_name_;
  uint8_t user_state_;
  SessionPtr session_;
  GameTable *table_;

  friend class GameTable;
  friend class ChessServer;
};

#endif /* GAMEUSER_H */
