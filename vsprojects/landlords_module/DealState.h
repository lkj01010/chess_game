#ifndef LANDLORDS_SERVER_DEAL_STATE_H
#define LANDLORDS_SERVER_DEAL_STATE_H

#include <vector>
#include <cstdint>
#include "GameState.h"
#include "landlords_proto.pb.h"

namespace landlords {

  class DealState : public GameState {
  public:
    DealState(LandlordsGame *game, IServerLogi *server);
    ~DealState();

  protected:
    uint16_t DoGetMessageSize(seat_id_t seat) override;

    uint16_t DoGetMessage(seat_id_t seat, void *buffer) override;

    void DoHandleMessage(IUser *user, uint16_t type, const void *mess, uint16_t sz) override;

  private:
    /** 发牌 */
    void Deal();

    /** 叫地主 */
    void HandleGrab(seat_id_t seat, const GrabLandlord &message);

    void InitGrabTimer();

    void OnTimeOut();

  private:
    std::vector<int> extra_; // 留给地主的底牌
    seat_id_t start_; // 从谁开始抢地主的
    seat_id_t cur_; // 当前由谁抢地主
    int8_t max_grade_; // 当前叫的最高分
    seat_id_t who_; // 谁叫的最高分
    timer_id_t timer_id_ = INVALID_TIMER_ID;
  };

}

#endif /* LANDLORDS_SERVER_DEAL_STATE_H */
