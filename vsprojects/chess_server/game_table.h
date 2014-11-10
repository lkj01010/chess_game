#ifndef GAMETABLE_H
#define GAMETABLE_H

#include <vector>
#include <cassert>
#include <boost/utility.hpp>
#include <chess_server/interfaces.h>
#include "game_user.h"

class ChessServer;

class GameTable : public IServerLogi, private boost::noncopyable {
public:
  GameTable();
  ~GameTable();

  void Init(ChessServer* server, int table_id, int seat_count);
  int table_id() const { return table_id_; }


  /** 座位是否没人 */
  bool IsEmptySeat(int seat) const {
    return users_[seat] == nullptr;
  }

  /** 设置座位上的玩家 */
  void SetUser(int pos, GameUserPtr user) {
    assert(pos >= 0 && pos < (int)users_.size());
    users_[pos] = user;
  }

  /** 获取这个玩家的座位号 */
  int GetSeat(GameUserPtr user) {
    for (int i = 0; i < (int)users_.size(); ++i) {
      if (users_[i] == user) { return i; }
    }
    return -1;
  }

  void BroadcastMessage(const void *mess, uint16_t sz) override;
  void BroadcastMessageExcept(seat_id_t seat_id, const void *mess, uint16_t sz) override;
  void SendMessageTo(seat_id_t seat_id, const void *mess, uint16_t sz) override;
  void SendMessageTo(seat_id_t seat_id) override {
    uint16_t sz = game_logi_->GetMessageSize(seat_id);
    if (!sz) { return; }
    std::unique_ptr<char[]> buffer(new char[sz]);
    game_logi_->GetMessageData(seat_id, buffer.get());
    SendMessageTo(seat_id, buffer.get(), sz);
  }
  timer_id_t SetTimeout(const std::function<void()> &callback, int delay) override { return -1; }
  void ClearTimeout(timer_id_t id) override {}
  timer_id_t SetInterval(const std::function<void()> &callback, int delay) override { return -1; }
  void ClearInterval(timer_id_t id) override {}

  void HandleMessage(GameUserPtr user, uint8_t type, const void* data, uint16_t sz);

private:
  void HandleSitdown(GameUserPtr user);
  void HandleStandup(GameUserPtr user);
  void HandleReady(GameUserPtr user);
  void HandleGame(GameUserPtr user, const void* data, uint16_t sz);

private:
  ChessServer* server_;
  int table_id_;
  IGameLogi *game_logi_;
  std::vector<GameUserPtr> users_;
};

#endif /* GAMETABLE_H */
