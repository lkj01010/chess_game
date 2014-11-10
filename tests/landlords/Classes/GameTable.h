#ifndef GAMETABLE_H
#define GAMETABLE_H

#include <vector>
#include <boost/utility.hpp>
#include "GameUser.h"

class GameUser;

class GameTable : boost::noncopyable {
public:
  GameTable();
  ~GameTable();

  void Init(int table_id, int seat_count) {
    table_id_ = table_id;
    users_.resize(seat_count);
  }

  void SetSeatUser(int seat, GameUserPtr user) {
    users_[seat] = user;
  }

  GameUserPtr GetUser(int seat_id) {
    return users_[seat_id];
  }

  int table_id() const { return table_id_; }

  int GetSeat(const GameUserPtr user) const {
    for (int i = 0; i < (int)users_.size(); ++i) {
      if (users_[i] == user) { return i; }
    }
    return -1;
  }

private:
  int table_id_;
  std::vector<GameUserPtr> users_;
};

#endif /* GAMETABLE_H */
