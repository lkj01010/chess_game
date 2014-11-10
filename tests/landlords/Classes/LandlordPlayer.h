#ifndef LANDLORDPLAYER_H
#define LANDLORDPLAYER_H

#include <vector>
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "CardContainer.h"
#include "Clock.h"

using namespace cocos2d;
using namespace cocos2d::ui;

struct LandlordPlayerUiConfig {
  LandlordPlayerUiConfig(Vec2 ready_pos,
                         bool is_hor,
                         Vec2 hand_pos,
                         Vec2 put_pos,
                         Vec2 header_pos,
                         Vec2 name_pos,
                         Vec2 role_pos) {
    this->ready_pos = ready_pos;
    this->is_hor = is_hor;
    this->hand_pos = hand_pos;
    this->put_pos = put_pos;
    this->header_pos = header_pos;
    this->name_pos = name_pos;
    this->role_pos = role_pos;
  }
  LandlordPlayerUiConfig() {}

  Vec2 ready_pos;
  bool is_hor;
  Vec2 hand_pos;
  Vec2 put_pos;
  Vec2 header_pos;
  Vec2 name_pos;
  Vec2 role_pos;
};

class LandlordPlayer : public Node {
  typedef Node Base;
public:
  static LandlordPlayer* create(Node* parent, const LandlordPlayerUiConfig& conf) {
    LandlordPlayer* object = new LandlordPlayer();
    if (!object) { return nullptr; }
    object->parent_ = parent;
    object->ui_config_ = conf;
    if (object->init()) {
      object->autorelease();
      return object;
    }
    delete object;
    return object;
  }

  LandlordPlayer();
  ~LandlordPlayer();
  bool init() override;

  void ShowReady(bool visiable) {
    ready_img_->setVisible(visiable);
  }
  void SetHandCards(const std::vector<int>& cards) {
    hand_cards_->SetCards(cards);
  }
  void SetPutCards(const std::vector<int>& cards) {
    put_cards_->SetCards(cards);
  }
  void ShowHeader(bool visiable) {
    header_img_->setVisible(visiable);
  }
  void SetName(const std::string& name) {
    name_label_->setString(name);
  }
  void SetRole(bool is_lord) {
    role_lord_img_->setVisible(is_lord);
    role_farmer_img_->setVisible(!is_lord);
  }
  void ShowClock(bool visiable) { clock_->setVisible(visiable); }

  CardContainer* hand_cards() const {
    return hand_cards_;
  }

  std::vector<int> GetSelCards() {
    return hand_cards_->GetSelectedCards();
  }
  void RemoveCards(const std::vector<int>& cards) {
    hand_cards_->Remove(cards);
  }

  void UserEnter();
  void UserLeave();

private:
  Node* parent_;
  LandlordPlayerUiConfig ui_config_;
  ImageView* ready_img_; // 准备标志
  CardContainer* hand_cards_; // 手上的牌
  CardContainer* put_cards_; // 上一次出的牌
  ImageView* header_img_; // 头像
  Label* name_label_; // 名字
  ImageView* role_lord_img_; // 地主标志
  ImageView* role_farmer_img_; // 农民标志
  Clock* clock_;

  friend class HelloWorld;
};

#endif /* LANDLORDPLAYER_H */
