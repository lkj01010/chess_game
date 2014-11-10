#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include <string>
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "GameLogi.h"
#include "LandlordPlayer.h"
#include "CardContainer.h"
#include "Clock.h"
#include "landlords_proto.pb.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace google::protobuf;


class CardContainer;

class HelloWorld : public cocos2d::Layer, public ConnectionMonitor, public GameLogi {
  enum {
    LEFT = 0,
    SELF = 1,
    RIGHT = 2
  };

public:
  CREATE_FUNC(HelloWorld);
  static cocos2d::Scene* createScene();
  virtual bool init();
  void OnConnected() override;
  void OnLogin() override;
  void OnDisconnected(bool connected) override;

  void OnUserEnter(GameUser* user) override;
  void OnUserStateChange(GameUser* user) override;
  void OnUserLeave(GameUser* user) override;
  void HandleMessage(const void* data, uint16_t sz) override;

private:
  void DoUiLogin(const std::string& uid, const std::string& passwd);
  void OnTouchReady(Ref *pSender, Widget::TouchEventType type);
  void OnTouchCallLord(Ref *pSender, Widget::TouchEventType type);
  void OnTouchPassCallLord(Ref *pSender, Widget::TouchEventType type);
  void OnTouchPutCard(Ref *pSender, Widget::TouchEventType type);
  void OnTouchPassPutCard(Ref *pSender, Widget::TouchEventType type);

  LandlordPlayer* GetPlayer(seat_id_t my_seat_id, seat_id_t seat_id);
  void ShowClock(LandlordPlayer* player);

  void SendProtoMessage(uint16_t type, const MessageLite* message);
  void HandDeal(DealCard& deal);
  void HandleGrabLandlord(GrabLandlord& grab);
  void HandleGrabLandlordOver(GrabLandlordOver& over);
  void HandleGameOver(GameOver& gameover);
  void HandlePlayCard(PlayCard& play);

private:
  GameClient client_;
  LandlordPlayer* players_[3];
  Clock *clock_;
  Button* ready_button_; // 准备按钮
  Button* call_lord_button_; // 叫地主按钮
  Button* pass_call_lord_button_; // 不叫地主按钮
  Button* put_card_button_; // 出牌按钮
  Button* pass_put_card_button_; // 不出按钮
  CardContainer* extra_cards_;

  std::vector<int> my_out_cards_;
};

#endif // __HELLOWORLD_SCENE_H__
