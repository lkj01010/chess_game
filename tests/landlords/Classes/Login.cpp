#include "Login.h"

Login::Login() {}


Login::~Login() {}


bool Login::init() {
  if (!Base::init()) { return false; }
  layout_ = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("login_ui/login_ui.json"));
  addChild(layout_);
  Button* login_button = static_cast<Button*>(Helper::seekWidgetByName(layout_, "login_button"));
  login_button->addTouchEventListener(CC_CALLBACK_2(Login::OnTouchLogin, this));
  return true;
}


void Login::OnTouchLogin(Ref *pSender, Widget::TouchEventType type) {
  if (type != Widget::TouchEventType::ENDED) { return; }
  TextField* uid_text = static_cast<TextField*>(Helper::seekWidgetByName(layout_, "uid_text"));
  TextField* passwd_text = static_cast<TextField*>(Helper::seekWidgetByName(layout_, "passwd_text"));
  const std::string uid = uid_text->getStringValue();
  const std::string passwd = passwd_text->getStringValue();
  if (uid.empty() || passwd.empty()) { return; }
  if (cb_) { cb_(uid, passwd); }
  this->runAction(CallFunc::create([&]() {
    removeFromParent();
  }));
}
