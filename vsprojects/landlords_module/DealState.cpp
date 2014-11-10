#include <cmath>
#include "DealState.h"
#include "LandlordsGame.h"
#include "PlayState.h"
#include "proto_helper.h"

namespace landlords {

  DealState::DealState(LandlordsGame *game, IServerLogi *server) : GameState(game, server) {
    Deal();
    start_ = cur_ = std::rand() % PLAYER_COUNT;
    max_grade_ = 0;
    who_ = PLAYER_COUNT;
    InitGrabTimer();
  }


  DealState::~DealState() {
    if (timer_id_ != INVALID_TIMER_ID) {
      server_->ClearTimeout(timer_id_);
    }
  }


  uint16_t DealState::DoGetMessageSize(seat_id_t seat) {
    return DoGetMessage(seat, nullptr);
  }


  uint16_t DealState::DoGetMessage(seat_id_t seat, void *buffer) {
    DealCard deal;
    std::vector<int> cards = game_->players_[seat].cards();
    for (auto val : cards) {
      deal.add_cards(val);
    }
    for (auto val : extra_) {
      deal.add_extras(val);
    }
    deal.set_cur(cur_);
    deal.set_max_grade(max_grade_);
    auto vec = SerializeProtoPacket(deal, MSG_DEAL);
    if (buffer) {
      std::memcpy(buffer, &vec[0], vec.size());
    }
    return vec.size();
  }


  void DealState::DoHandleMessage(IUser *user, uint16_t type, const void *mess, uint16_t sz) {
    if (type == MSG_GRAB) {
      GrabLandlord message;
      if (message.ParseFromArray(mess, sz)) {
        HandleGrab(user->seat_id(), message);
      }
    }
  }


  void DealState::Deal() {
    const int kCardCount = 54;
    const int kDealCount = (kCardCount - EXTRA_CARD_COUNT) / PLAYER_COUNT;
    std::vector<int> cards(kCardCount);
    for (std::vector<int>::size_type i = 0; i < cards.size(); ++i) {
      cards[i] = i;
    }
    std::random_shuffle(cards.begin(), cards.end());
    uint32_t next = 0;
    for (int i = 0; i < PLAYER_COUNT; ++i) {
      std::vector<int> player_card(cards.begin() + next, cards.begin() + next + kDealCount);
      std::sort(player_card.begin(), player_card.end());
      game_->players_[i].set_cards(player_card);
      next += kDealCount;
    }
    extra_.clear();
    for (int i = 0; i < EXTRA_CARD_COUNT; ++i) {
      assert(next < cards.size());
      extra_.push_back(cards[next++]);
    }
    std::sort(extra_.begin(), extra_.end());
  }


  void DealState::HandleGrab(seat_id_t seat, const GrabLandlord &message) {
    if (seat != cur_) {
      return;
    }
    cur_ = (cur_ + 1) % PLAYER_COUNT;
    if (message.grade() > max_grade_) {
      max_grade_ = message.grade();
      who_ = seat;
    }
    if (3 == max_grade_ || (cur_ == start_ && who_ < PLAYER_COUNT)) {
      // 叫地主结束，开始打牌
      GrabLandlordOver over;
      over.set_who(who_);
      over.set_grade(max_grade_);
      auto vec = SerializeProtoPacket(over, MSG_GRAB_DONE);
      server_->BroadcastMessage(&vec[0], vec.size());
      game_->SetState(std::unique_ptr<PlayState>(new PlayState(game_, server_, who_, extra_)));
    } else if (cur_ == start_ && who_ == PLAYER_COUNT) {
      // 没人叫地主，本局结束
      GameOver over;
      auto vec = SerializeProtoPacket(over, MSG_OVER);
      server_->BroadcastMessage(&vec[0], vec.size());
    } else {
      // 让下个人叫
      GrabLandlord grab = message;
      grab.set_cur(cur_);
      auto vec = SerializeProtoPacket(grab, MSG_GRAB);
      server_->BroadcastMessage(&vec[0], vec.size());
      InitGrabTimer();
    }
  }


  void DealState::InitGrabTimer() {
    if (timer_id_ != INVALID_TIMER_ID) {
      server_->ClearTimeout(timer_id_);
      timer_id_ = INVALID_TIMER_ID;
    }
    timer_id_ = server_->SetTimeout(std::bind(&DealState::OnTimeOut, this), 30000);
  }


  void DealState::OnTimeOut() {
    GrabLandlord message;
    message.set_grade(0);
    HandleGrab(cur_, message);
  }

}
