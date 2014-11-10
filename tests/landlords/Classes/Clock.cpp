#include <string.h>
#include "Clock.h"

using namespace cocos2d;

bool Clock::init() {
  if (!Base::init()) { return false; }
  auto bg = Sprite::create("lord_clock_bg.png");
  addChild(bg);
  nums_ = Node::create();
  addChild(nums_);
  SetSeconds(0);
  return true;
}


void Clock::SetSeconds(int seconds) {
  seconds_ = seconds + 1;
  this->runAction(RepeatForever::create(Sequence::create(
    CCCallFunc::create([&]() {this->OnTimer(); }),
    DelayTime::create(1.0f),
    nullptr)));
}


void Clock::OnTimer() {
  seconds_ -= 1;
  nums_->removeAllChildren();
  int h = seconds_ / 10;
  int l = seconds_ % 10;
  char path[32];
  sprintf(path, "lord_cards_num_%d.png", h);
  auto sp = Sprite::create(path);
  nums_->addChild(sp);
  sp->setPosition(Vec2(-sp->getContentSize().width / 2, -3));

  sprintf(path, "lord_cards_num_%d.png", l);
  sp = Sprite::create(path);
  nums_->addChild(sp);
  sp->setPosition(Vec2(sp->getContentSize().width / 2, -3));

  if (seconds_ <= 0) {
    this->stopAllActions();
  }
}
