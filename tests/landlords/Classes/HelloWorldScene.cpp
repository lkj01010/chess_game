#include <Poco/ByteOrder.h>
#include "HelloWorldScene.h"
#include "CardContainer.h"
#include "Login.h"
#include "landlords_proto.pb.h"

enum {
  MSG_DEAL = 0,
  MSG_GRAB,
  MSG_GRAB_DONE,
  MSG_PLAY,
  MSG_OVER,
  MSG_END
};


Scene* HelloWorld::createScene() {
  // 'scene' is an autorelease object
  auto scene = Scene::create();

  // 'layer' is an autorelease object
  auto layer = HelloWorld::create();

  // add layer as a child to scene
  scene->addChild(layer);

  // return the scene
  return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init() {
  if (!Layer::init()) {
    return false;
  }

  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();
  
  auto sprite = Sprite::create("Background.jpg");
  sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
  this->addChild(sprite, 0);

  players_[LEFT] = LandlordPlayer::create(this, LandlordPlayerUiConfig(Vec2(51, 300),
    false, Vec2(160, 430), Vec2(260, 430), Vec2(51, 435), Vec2(51, 380), Vec2(51, 500)));
  addChild(players_[LEFT]);
  players_[RIGHT] = LandlordPlayer::create(this, LandlordPlayerUiConfig(Vec2(729, 300),
    false, Vec2(620, 430), Vec2(520, 430), Vec2(729, 435), Vec2(729, 380), Vec2(729, 500)));
  addChild(players_[RIGHT]);
  players_[SELF] = LandlordPlayer::create(this, LandlordPlayerUiConfig(Vec2(118, 25),
    true, Vec2(410, 80), Vec2(410, 260), Vec2(118, 110), Vec2(118, 55), Vec2(118, 175)));
  addChild(players_[SELF]);
  players_[LEFT]->setVisible(false);
  players_[RIGHT]->setVisible(false);
  players_[SELF]->setVisible(false);

  extra_cards_ = CardContainer::CreatefHorizontal(25, 0, false);
  addChild(extra_cards_);
  extra_cards_->setScale(0.70f);
  extra_cards_->setPosition(Vec2(visibleSize.width / 2, 635));
  
  ready_button_ = Button::create("button/Ready_01.png", "button/Ready_03.png", "button/Ready_04.png");
  addChild(ready_button_);
  ready_button_->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
  ready_button_->addTouchEventListener(CC_CALLBACK_2(HelloWorld::OnTouchReady, this));

  call_lord_button_ = Button::create("button/CallLord_01.png", "button/CallLord_02.png", "button/CallLord_04.png");
  addChild(call_lord_button_);
  call_lord_button_->setPosition(Vec2(visibleSize.width / 2 - 50, 190));
  call_lord_button_->addTouchEventListener(CC_CALLBACK_2(HelloWorld::OnTouchCallLord, this));
  call_lord_button_->setVisible(false);

  pass_call_lord_button_ = Button::create("button/PassCallLord_01.png", "button/PassCallLord_02.png", "button/PassCallLord_04.png");
  addChild(pass_call_lord_button_);
  pass_call_lord_button_->setPosition(Vec2(visibleSize.width / 2 + 50, 190));
  pass_call_lord_button_->addTouchEventListener(CC_CALLBACK_2(HelloWorld::OnTouchPassCallLord, this));
  pass_call_lord_button_->setVisible(false);

  put_card_button_ = Button::create("button/PutCards_01.png", "button/PutCards_02.png", "button/PutCards_04.png");
  addChild(put_card_button_);
  put_card_button_->setPosition(Vec2(visibleSize.width / 2 - 50, 190));
  put_card_button_->addTouchEventListener(CC_CALLBACK_2(HelloWorld::OnTouchPutCard, this));
  put_card_button_->setVisible(false);

  pass_put_card_button_ = Button::create("button/PassPutCards_01.png", "button/PassPutCards_02.png", "button/PassPutCards_04.png");
  addChild(pass_put_card_button_);
  pass_put_card_button_->setPosition(Vec2(visibleSize.width / 2 + 50, 190));
  pass_put_card_button_->addTouchEventListener(CC_CALLBACK_2(HelloWorld::OnTouchPassPutCard, this));
  pass_put_card_button_->setVisible(false);

  // login_ui
  auto login_ui = Login::create(std::bind(&HelloWorld::DoUiLogin, this,
    std::placeholders::_1, std::placeholders::_2));
  addChild(login_ui);
  client_.SetMonitor(this);
  client_.SetGameLogi(this);
  client_.Start("127.0.0.1", "5678");
  return true;
}


void HelloWorld::DoUiLogin(const std::string& uid, const std::string& passwd) {
  client_.SendLogin(uid, passwd);
}


void HelloWorld::OnTouchReady(Ref *pSender, Widget::TouchEventType type) {
  if (type != Widget::TouchEventType::ENDED) { return; }
  client_.SendReady();
}


void HelloWorld::OnTouchCallLord(Ref *pSender, Widget::TouchEventType type) {
  if (type != Widget::TouchEventType::ENDED) { return; }
  GrabLandlord message;
  // TODO: 可以选择叫1分，2分，3分
  message.set_grade(3);
  SendProtoMessage(MSG_GRAB, &message);
}


void HelloWorld::OnTouchPassCallLord(Ref *pSender, Widget::TouchEventType type) {
  if (type != Widget::TouchEventType::ENDED) { return; }
  GrabLandlord message;
  message.set_grade(0);
  SendProtoMessage(MSG_GRAB, &message);
}


void HelloWorld::OnTouchPutCard(Ref *pSender, Widget::TouchEventType type) {
  if (type != Widget::TouchEventType::ENDED) { return; }
  PlayCard pc;
  my_out_cards_ = players_[SELF]->GetSelCards();
  for (auto v : my_out_cards_) {
    pc.add_cards(v);
  }
  SendProtoMessage(MSG_PLAY, &pc);
}


void HelloWorld::OnTouchPassPutCard(Ref *pSender, Widget::TouchEventType type) {
  if (type != Widget::TouchEventType::ENDED) { return; }
  my_out_cards_.clear();
  players_[SELF]->hand_cards()->Deselect();
  PlayCard pc;
  SendProtoMessage(MSG_PLAY, &pc);
}

void HelloWorld::OnConnected() {
  CCLOG("connect to server ok.");
}

void HelloWorld::OnLogin() {
  CCLOG("login succeed");
  client_.SendRandSeatDown();
}

void HelloWorld::OnDisconnected(bool connected) {
  if (connected) {
    CCLOG("connection lost");
  } else {
    CCLOG("connect to server failed.");
  }
}


void HelloWorld::OnUserEnter(GameUser* user) {
  CCLOG("OnUserEnter");
  // 目前GameUser不是线程安全的，所以先复制一份
  std::string nick_name = user->nick_name();
  int my_seat_id = client_.GetMyself()->seat_id();
  seat_id_t seat_id = user->seat_id();
  this->getScheduler()->performFunctionInCocosThread([=]() {
    LandlordPlayer* player = GetPlayer(my_seat_id, seat_id);
    player->SetName(nick_name);
    player->name_label_->setVisible(true);
    player->ShowHeader(true);
  });
}


void HelloWorld::OnUserStateChange(GameUser* user) {
  int my_seat_id = client_.GetMyself()->seat_id();
  seat_id_t seat_id = user->seat_id();
  this->getScheduler()->performFunctionInCocosThread([=]() {
    if (my_seat_id == seat_id) {
      ready_button_->setVisible(false);
    }
    LandlordPlayer* player = GetPlayer(my_seat_id, seat_id);
    player->ShowReady(true);
  });
}


void HelloWorld::OnUserLeave(GameUser* user) {
  int my_seat_id = client_.GetMyself()->seat_id();
  seat_id_t seat_id = user->seat_id();
  this->getScheduler()->performFunctionInCocosThread([=]() {
    LandlordPlayer* player = GetPlayer(my_seat_id, seat_id);
    player->UserLeave();
  });
}


void HelloWorld::HandleMessage(const void* data, uint16_t sz) {
  uint16_t message_type = *(const uint16_t*)data;
  message_type = Poco::ByteOrder::fromNetwork(message_type);
  if (message_type == MSG_DEAL) {
    DealCard deal;
    bool ret = deal.ParseFromArray((char*)data + 2, sz - 2);
    if (!ret) { return; }
    HandDeal(deal);
  } else if (message_type == MSG_GRAB_DONE) {
    GrabLandlordOver over;
    bool ret = over.ParseFromArray((char*)data + 2, sz - 2);
    if (!ret) { return; }
    HandleGrabLandlordOver(over);
  } else if (message_type == MSG_GRAB) {
    GrabLandlord grab;
    bool ret = grab.ParseFromArray((char*)data + 2, sz - 2);
    if (!ret) { return; }
    HandleGrabLandlord(grab);
  } else if (message_type == MSG_OVER) {
    GameOver gameover;
    bool ret = gameover.ParseFromArray((char*)data + 2, sz - 2);
    if (!ret) { return; }
    HandleGameOver(gameover);
  } else if (message_type == MSG_PLAY) {
    PlayCard play;
    bool ret = play.ParseFromArray((char*)data + 2, sz - 2);
    if (!ret) { return; }
    HandlePlayCard(play);
  }
}


LandlordPlayer* HelloWorld::GetPlayer(seat_id_t my_seat_id, seat_id_t seat_id) {
  if (seat_id == my_seat_id) {
    return players_[SELF];
  } else if ((my_seat_id + 1) % 3 == seat_id) {
    return players_[RIGHT];
  } else if ((my_seat_id + 2) % 3 == seat_id) {
    return players_[LEFT];
  }
  return nullptr;
}


void HelloWorld::ShowClock(LandlordPlayer* player) {
  for (LandlordPlayer* p : players_) {
    p->ShowClock(p == player);
  }
}


void HelloWorld::SendProtoMessage(uint16_t type, const MessageLite* message) {
  uint16_t sz = sizeof(type);
  if (message) { sz += message->ByteSize(); }
  std::unique_ptr<char[]> buffer(new char[sz]);
  *(uint16_t*)buffer.get() = type;
  if (message) {
    bool ret = message->SerializeToArray(&buffer[2], sz - 2);
    if (!ret) { abort(); }
  }
  client_.SendCommand(buffer.get(), sz);
}


void HelloWorld::HandDeal(DealCard& deal) {
  int who = deal.cur();
  int myseat = client_.GetMyself()->seat_id();
  this->getScheduler()->performFunctionInCocosThread([this, deal, who, myseat]() {
    std::vector<int> cards(deal.cards_size());
    for (int i = 0; i < (int)cards.size(); ++i) {
      cards[i] = deal.cards(i);
    }
    std::vector<int> extras(deal.extras_size());
    for (int i = 0; i < (int)extras.size(); ++i) {
      extras[i] = deal.extras(i);
    }
    LandlordPlayer* player = players_[SELF];
    player->SetHandCards(cards);
    extra_cards_->SetCards(extras);
    ShowClock(GetPlayer(myseat, who));
    if (myseat == who) {
      call_lord_button_->setVisible(true);
      pass_call_lord_button_->setVisible(true);
    } else {
      call_lord_button_->setVisible(false);
      pass_call_lord_button_->setVisible(false);
    }
  });
}


void HelloWorld::HandleGrabLandlord(GrabLandlord& grab) {
  int cur = grab.cur();
  int myseat = client_.GetMyself()->seat_id();
  this->getScheduler()->performFunctionInCocosThread([=]() {
    ShowClock(GetPlayer(myseat, cur));
    if (cur == myseat) {
      call_lord_button_->setVisible(true);
      pass_call_lord_button_->setVisible(true);
    } else {
      call_lord_button_->setVisible(false);
      pass_call_lord_button_->setVisible(false);
    }
  });
}


void HelloWorld::HandleGrabLandlordOver(GrabLandlordOver& over) {
  int myseat = client_.GetMyself()->seat_id();
  int who = over.who(); // 地主
  this->getScheduler()->performFunctionInCocosThread([=]() {
    call_lord_button_->setVisible(false);
    pass_call_lord_button_->setVisible(false);
    auto lord = GetPlayer(myseat, who);
    for (auto p : players_) {
      p->ShowClock(p == lord);
    }
    if (myseat == who) {
      put_card_button_->setVisible(true);
      pass_put_card_button_->setVisible(true);
    } else {
      put_card_button_->setVisible(false);
      pass_put_card_button_->setVisible(false);
    }
  });
}


void HelloWorld::HandleGameOver(GameOver& gameover) {
  this->getScheduler()->performFunctionInCocosThread([=]() {
    ready_button_->setVisible(true);
  });
}


void HelloWorld::HandlePlayCard(PlayCard& play) {
  int myseat = client_.GetMyself()->seat_id();
  std::vector<int> cards;
  for (int i = 0; i < play.cards_size(); ++i) {
    cards.push_back(play.cards(i));
  }
  int who = play.who();
  this->getScheduler()->performFunctionInCocosThread([=]() {
    GetPlayer(myseat, who)->SetPutCards(cards);
    if (myseat == who && !cards.empty()) {
      GetPlayer(myseat, who)->RemoveCards(my_out_cards_);
    }
    int next = (who + 1) % 3;
    auto nextp = GetPlayer(myseat, next);
    for (auto p : players_) {
      p->ShowClock(p == nextp);
    }
    put_card_button_->setVisible(nextp == players_[SELF]);
    pass_put_card_button_->setVisible(nextp == players_[SELF]);
    if (nextp == players_[SELF]) {
      players_[SELF]->SetPutCards(std::vector<int>());
    }
  });
}
