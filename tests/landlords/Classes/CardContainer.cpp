#include "CardContainer.h"

using namespace cocos2d;
using namespace cocos2d::ui;

const char* card_path[] = {
  "card/ht_3.png",
  "card/ht_4.png",
  "card/ht_5.png",
  "card/ht_6.png",
  "card/ht_7.png",
  "card/ht_8.png",
  "card/ht_9.png",
  "card/ht_10.png",
  "card/ht_J.png",
  "card/ht_Q.png",
  "card/ht_K.png",
  "card/ht_A.png",
  "card/ht_2.png",
  "card/hx_3.png",
  "card/hx_4.png",
  "card/hx_5.png",
  "card/hx_6.png",
  "card/hx_7.png",
  "card/hx_8.png",
  "card/hx_9.png",
  "card/hx_10.png",
  "card/hx_J.png",
  "card/hx_Q.png",
  "card/hx_K.png",
  "card/hx_A.png",
  "card/hx_2.png",
  "card/fk_3.png",
  "card/fk_4.png",
  "card/fk_5.png",
  "card/fk_6.png",
  "card/fk_7.png",
  "card/fk_8.png",
  "card/fk_9.png",
  "card/fk_10.png",
  "card/fk_J.png",
  "card/fk_Q.png",
  "card/fk_K.png",
  "card/fk_A.png",
  "card/fk_2.png",
  "card/mh_3.png",
  "card/mh_4.png",
  "card/mh_5.png",
  "card/mh_6.png",
  "card/mh_7.png",
  "card/mh_8.png",
  "card/mh_9.png",
  "card/mh_10.png",
  "card/mh_J.png",
  "card/mh_Q.png",
  "card/mh_K.png",
  "card/mh_A.png",
  "card/mh_2.png",
  "card/xiaowang.png",
  "card/dawang.png",
  "card/back.png"
};

CardContainer::CardContainer() {
  batch_insert_ = false;
}


CardContainer::~CardContainer() {
}


bool CardContainer::init() {
  if (!Base::init()) { return false; }
  return true;
}


void CardContainer::OnTouch(Ref *pSender, Widget::TouchEventType type) {
  if (type != Widget::TouchEventType::ENDED) { return; }
  CardView *view = (CardView*)pSender;
  auto pos = Vec2(0, 0);
  auto sz = this->getContentSize();
  if (view->selected) {
    view->selected = false;
    if (is_horizontal_) {
      pos.y = -margin_;
    } else {
      pos.x = -margin_;
    }
  } else {
    view->selected = true;
    if (is_horizontal_) {
      pos.y = margin_;
    } else {
      pos.x = margin_;
    }
  }
  view->stopAllActions();
  view->runAction(MoveBy::create(0.1f, pos));
}


void CardContainer::SetCards(const std::vector<int>& cards) {
  for (CardView* view : views_) {
    view->removeFromParent();
  }
  views_.clear();
  batch_insert_ = true;
  for (int value : cards) {
    PushCard(value);
  }
  batch_insert_ = false;
  AdjustCardPosition(false);
}


void CardContainer::PushCard(int card) {
  InsertCard((int)views_.size(), card);
}


void CardContainer::InsertCard(int pos, int card) {
  if (pos > (int)views_.size()) { return; }
  const int sz = sizeof(card_path) / sizeof(card_path[0]);
  assert(card < sz);
  auto view = CardView::create(card_path[card]);
  addChild(view);
  view->value = card;
  view->setAnchorPoint(Vec2(0, 0));
  view->setTouchEnabled(touchable_);
  view->addTouchEventListener(CC_CALLBACK_2(CardContainer::OnTouch, this));
  views_.insert(views_.begin() + pos, view);
  if (!batch_insert_) {
    AdjustCardPosition(true);
  }
}


int CardContainer::GetCardSize() const {
  return (int)views_.size();
}


std::vector<int> CardContainer::GetCards() const {
  std::vector<int> ret;
  for (auto view : views_) {
    ret.push_back(view->value);
  }
  return ret;
}


std::vector<int> CardContainer::GetSelectedCards() const {
  std::vector<int> ret;
  for (auto view : views_) {
    if (view->selected) {
      ret.push_back(view->value);
    }
  }
  return ret;
}


void CardContainer::AdjustCardPosition(bool animate) {
  if (views_.empty()) { return; }
  auto view = views_[0];
  const float width = view->getSize().width;
  const float height = view->getSize().height;
  float total_width = width;
  float total_height = height;
  const int count = (int)views_.size();
  if (count > 1) {
    if (is_horizontal_) {
      total_width += margin_ * (count - 1);
    } else {
      total_height += margin_*(count - 1);
    }
  }

  float start_x = -total_width / 2;
  float start_y = -total_height / 2;
  if (count > 1 && !is_horizontal_) { start_y += margin_ * (count - 1); }

  setContentSize(Size(total_width, total_height));

  int zorder = 0;
  for (auto view : views_) {
    view->setLocalZOrder(zorder++);
    auto pos = Vec2(start_x, start_y);
    if (view->selected) {
      if (is_horizontal_) {
        pos.y += margin_;
      } else {
        pos.x += margin_;
      }
    }
    if (animate) {
      view->stopAllActions();
      view->runAction(MoveTo::create(0.1f, pos));
    } else {
      view->setPosition(pos);
    }
    if (is_horizontal_) {
      start_x += margin_;
    } else {
      start_y -= margin_;
    }
  }
}


void CardContainer::Select(int pos) {
  if (pos < (int)views_.size()) {
    CardView* view = views_[pos];
    if (!view->selected) {
      view->selected = true;
      AdjustCardPosition(true);
    }
  }
}


void CardContainer::Select(const std::vector<int>& pos) {
  for (auto value : pos) {
    Select(value);
  }
}


void CardContainer::Deselect() {
  for (int pos = 0; (int)pos < views_.size(); ++pos) {
    if (views_[pos]->selected) {
      Deselect(pos);
    }
  }
}


void CardContainer::Deselect(int pos) {
  if (pos < (int)views_.size()) {
    CardView* view = views_[pos];
    if (view->selected) {
      view->selected = false;
      AdjustCardPosition(true);
    }
  }
}


void CardContainer::Deselect(const std::vector<int>& pos) {
  for (auto value : pos) {
    Deselect(value);
  }
}


void CardContainer::Remove(const std::vector<int>& cards) {
  for (auto card : cards) {
    for (auto iter = views_.begin(); iter != views_.end(); ++iter) {
      if ((*iter)->value == card) {
        (*iter)->removeFromParent();
        views_.erase(iter);
        break;
      }
    }
  }
  AdjustCardPosition(true);
}
