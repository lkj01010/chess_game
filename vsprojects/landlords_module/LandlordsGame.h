#ifndef LANDLORDS_LANDLORDSGAME_H
#define LANDLORDS_LANDLORDSGAME_H

#include <vector>
#include <cassert>
#include <cstdint>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <chess_server/interfaces.h>
#include "GameState.h"
#include "LandlordsPlayer.h"

namespace landlords {

  class LandlordsGame : public IGameLogi {
  public:
    LandlordsGame(IServerLogi* server_logi);
    ~LandlordsGame();

    bool OnGameStart() override;
    void HandleMessage(IUser *user, const void *mess, uint16_t sz) override;
    uint16_t GetMessageSize(seat_id_t seat) override;
    void GetMessageData(seat_id_t seat, void *buffer) override;

    void SetState(std::unique_ptr<GameState> &&state) {
      game_state_ = std::move(state);
    }

  private:

  private:
    std::unique_ptr<GameState> game_state_;
    LandlordsPlayer players_[PLAYER_COUNT];
    IServerLogi* const server_logi_;
    
    friend class GameState;
    friend class DealState;
    friend class PlayState;
  };

}
#endif /* LANDLORDS_LANDLORDSGAME_H */
