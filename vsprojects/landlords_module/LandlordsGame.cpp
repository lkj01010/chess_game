#include <chess_server/interfaces.h>
#include "LandlordsGame.h"
#include "DealState.h"

namespace landlords {

  LandlordsGame::LandlordsGame(IServerLogi* server_logi) : server_logi_(server_logi) {
  }


  LandlordsGame::~LandlordsGame() {
  }


  bool LandlordsGame::OnGameStart() {
    SetState(std::unique_ptr<GameState>(new DealState(this, server_logi_)));
    return true;
  }


  void LandlordsGame::HandleMessage(IUser *user, const void *mess, uint16_t sz) {
    game_state_->HandleMessage(user, mess, sz);
  }


  uint16_t LandlordsGame::GetMessageSize(seat_id_t seat) {
    return game_state_->GetMessageSize(seat);
  }


  void LandlordsGame::GetMessageData(seat_id_t seat, void *buffer) {
    game_state_->GetMessage(seat, buffer);
  }

}
