#ifndef LOGIN_H
#define LOGIN_H

#include <functional>
#include <string>
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

using namespace cocos2d;
using namespace cocos2d::ui;

class Login : public cocos2d::Node {
  typedef cocos2d::Node Base;
public:
  typedef std::function<void(const std::string&, const std::string&)> login_callback_t;

  static Login* create(const login_callback_t& cb) {
    Login* inst = new Login();
    if (!inst) { return NULL; }
    inst->cb_ = cb;
    if (inst->init()) {
      inst->autorelease();
      return inst;
    }
    delete inst;
    return nullptr;
  }

  Login();
  ~Login();
  bool init() override;


private:
  void OnTouchLogin(Ref *pSender, Widget::TouchEventType type);

private:
  Layout* layout_;
  login_callback_t cb_;
};

#endif /* LOGIN_H */
