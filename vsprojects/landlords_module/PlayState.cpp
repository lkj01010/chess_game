#include "PlayState.h"
#include "proto_helper.h"
#include "LandlordsGame.h"

namespace landlords {

  PlayState::PlayState(LandlordsGame *game, IServerLogi *server, seat_id_t master, const std::vector<int> &extra)
    : GameState(game, server) {
    master_ = cur_ = master;
    extra_ = extra;
  }


  PlayState::~PlayState() {
  }


  uint16_t PlayState::DoGetMessage(seat_id_t seat, void *buffer) {
    return 0;
  }


  void PlayState::DoHandleMessage(IUser *user, uint16_t type, const void *mess, uint16_t sz) {
    if (type == MSG_PLAY) {
      PlayCard message;
      if (message.ParseFromArray(mess, sz)) {
        HandlePlay(user, message);
      }
    }
  }


  void PlayState::HandlePlay(IUser *user, const PlayCard &message) {
    if (cur_ != user->seat_id()) { return; }
    std::vector<int> values;
    for (int i = 0; i < message.cards_size(); ++i) {
      values.push_back(message.cards(i));
    }
    if (values.empty()) {
      if (last_ == PLAYER_COUNT) { return; } // �����������Ҫ
      // �ͻ��˿��Լ��㣺1��������˭���ģ� 2���¸�˭����
      PlayCard play;
      play.set_who(user->seat_id());
      auto vec = SerializeProtoPacket(play, MSG_PLAY);
      server_->BroadcastMessage(&vec[0], vec.size());
    } else {
      if (!game_->players_[user->seat_id()].have_cards(values)) { return; } // ������ô��
      LandlordsCard cards;
      cards.SetCards(values);
      if (CardType::INVALID == cards.card_type()) { return; }
      if (last_ != PLAYER_COUNT && !(cards > last_card_)) { return; } // û�ϼ��ƴ�
      PlayCard play;
      for (auto value : values) {
        play.add_cards(value);
      }
      play.set_who(user->seat_id());
      auto vec = SerializeProtoPacket(play, MSG_PLAY);
      server_->BroadcastMessage(&vec[0], vec.size());
      last_ = user->seat_id();
      last_card_ = std::move(cards);
      game_->players_[user->seat_id()].rm_cards(values);
      if (game_->players_[user->seat_id()].no_cards()) {
        // �Ƴ����ˣ���Ϸ����
        GameOver over;
        auto vec = SerializeProtoPacket(over, MSG_OVER);
        server_->BroadcastMessage(&vec[0], vec.size());
      }
    }
    cur_ = (cur_ + 1) % PLAYER_COUNT;
    if (cur_ == last_) {
      last_ = PLAYER_COUNT;
    }
  }

}
