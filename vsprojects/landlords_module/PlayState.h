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
    /** ���� */
    void HandlePlay(IUser *user, const PlayCard &message);

  private:
    seat_id_t master_; // ����
    std::vector<int> extra_; // �����Ĳ���

    seat_id_t last_ = PLAYER_COUNT; // ��һ�����Ƶ����
    LandlordsCard last_card_; // ��һ����ҳ�����
    seat_id_t cur_; // ��ǰ�ֵ�˭������
  };

}

#endif /* LANDLORDS_SERVER_PLAY_STATE_H */
