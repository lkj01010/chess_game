#ifndef LANDLORDS_SERVER_PLAY_STATE_H
#define LANDLORDS_SERVER_PLAY_STATE_H

#include <vector>
#include <chess_server/interfaces.h>
#include "GameState.h"
#include "landlords_proto.pb.h"
#include "LandlordsCard.h"

namespace landlords {

  class PlayState : public GameState {
  public:
    PlayState(LandlordsGame *game, IServerLogi *server, seat_id_t master, const std::vector<int> &extra);
    ~PlayState();

  protected:
    uint16_t DoGetMessageSize(seat_id_t seat) override {
      return DoGetMessage(seat, nullptr);
    }

    uint16_t DoGetMessage(seat_id_t seat, void *buffer) override;

    void DoHandleMessage(IUser *user, uint16_t type, const void *mess, uint16_t sz) override;

  private:
    /** 出牌 */
    void HandlePlay(IUser *user, const PlayCard &message);

  private:
    seat_id_t master_; // 地主
    std::vector<int> extra_; // 地主的补牌

    seat_id_t last_ = PLAYER_COUNT; // 上一个出牌的玩家
    LandlordsCard last_card_; // 上一个玩家出的牌
    seat_id_t cur_; // 当前轮到谁出牌了
  };

}

#endif /* LANDLORDS_SERVER_PLAY_STATE_H */
