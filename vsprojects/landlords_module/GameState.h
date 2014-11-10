#ifndef LANDLORDS_SERVER_GAME_STATE_H
#define LANDLORDS_SERVER_GAME_STATE_H

#include <cstdint>
#include <cassert>
#include <chess_server/interfaces.h>

namespace landlords {

  //enum {
  //  STATE_GRAB,
  //  STATE_PLAY,
  //  STATE_OVER
  //};

  enum {
    MSG_DEAL = 0,
    MSG_GRAB,
    MSG_GRAB_DONE,
    MSG_PLAY,
    MSG_OVER,
    MSG_END
  };

  enum { PLAYER_COUNT = 3 };

  enum { EXTRA_CARD_COUNT = PLAYER_COUNT };

  class LandlordsGame;

  class GameState {
  public:
    GameState(LandlordsGame *game, IServerLogi *server) {
      game_ = game;
      server_ = server;
    }
    virtual ~GameState() {}

    void HandleMessage(IUser *user, const void *mess, uint16_t sz) {
      assert(sz >= sizeof(uint16_t));
      const uint16_t message_type = *(const uint16_t*)mess;
      const void* message = (const char*)mess + 2;
      const uint16_t message_sz = sz - 2;
      DoHandleMessage(user, message_type, message, message_sz);
    }

    uint16_t GetMessageSize(seat_id_t seat) {
      return DoGetMessageSize(seat);
    }

    uint16_t GetMessage(seat_id_t seat, void *buffer) {
      return DoGetMessage(seat, buffer);
    }

  protected:
    virtual uint16_t DoGetMessageSize(seat_id_t seat) = 0;
    virtual uint16_t DoGetMessage(seat_id_t seat, void *buffer) = 0;
    virtual void DoHandleMessage(IUser *user, uint16_t type, const void *mess, uint16_t sz) = 0;

  protected:
    LandlordsGame *game_;
    IServerLogi *server_;
  };

}

#endif /* LANDLORDS_SERVER_GAME_STATE_H */
