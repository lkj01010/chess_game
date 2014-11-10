#include "LandlordPlayer.h"

LandlordPlayer::LandlordPlayer() {}


LandlordPlayer::~LandlordPlayer() {}


bool LandlordPlayer::init() {
  if (!Base::init()) { return false; }
  if (ui_config_.is_hor) {
    hand_cards_ = CardContainer::CreatefHorizontal(25, 30, true);
    put_cards_ = CardContainer::CreatefHorizontal(25, 30, false);
    hand_cards_->setScale(0.8f);
    put_cards_->setScale(0.6f);
  } else {
    hand_cards_ = CardContainer::CreatefVertical(25, 30, false);
    put_cards_ = CardContainer::CreatefVertical(25, 30, false);
    hand_cards_->setScale(0.6f);
    put_cards_->setScale(0.6f);
  }
  parent_->addChild(hand_cards_);
  hand_cards_->setPosition(ui_config_.hand_pos);
  parent_->addChild(put_cards_);
  put_cards_->setPosition(ui_config_.put_pos);

  header_img_ = ImageView::create("header.png");
  parent_->addChild(header_img_);
  header_img_->setPosition(ui_config_.header_pos);
  header_img_->setVisible(false);

  clock_ = Clock::create();
  parent_->addChild(clock_);
  clock_->setPosition(ui_config_.header_pos);
  clock_->setVisible(false);

  name_label_ = Label::createWithTTF("Neo", "fonts/MarkerFelt.ttf", 16);
  parent_->addChild(name_label_);
  name_label_->setPosition(ui_config_.name_pos);
  name_label_->setVisible(false);

  role_lord_img_ = ImageView::create("LordMark.png");
  parent_->addChild(role_lord_img_);
  role_lord_img_->setPosition(ui_config_.role_pos);
  role_lord_img_->setVisible(false);

  role_farmer_img_ = ImageView::create("FarmerMark.png");
  parent_->addChild(role_farmer_img_);
  role_farmer_img_->setPosition(ui_config_.role_pos);
  role_farmer_img_->setVisible(false);

  ready_img_ = ImageView::create("ready.png");
  parent_->addChild(ready_img_);
  ready_img_->setPosition(ui_config_.ready_pos);
  ready_img_->setVisible(false);

  //hand_cards_->SetCards({ 0, 1, 2, 3, 4, 5, 6, 7, 8,9,10,11,12,13,14,15,16,17,18,19 });
  //put_cards_->SetCards({ 9, 10, 11 });
  return true; 
}

void LandlordPlayer::UserEnter() {}

void LandlordPlayer::UserLeave() {
  ready_img_->setVisible(false);
  hand_cards_->setVisible(false);
  put_cards_->setVisible(false);
  header_img_->setVisible(false);
  name_label_->setVisible(false);
  role_lord_img_->setVisible(false);
  role_farmer_img_->setVisible(false);
}
