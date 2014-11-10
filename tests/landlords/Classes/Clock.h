#ifndef CLOCK_H
#define CLOCK_H

#include "cocos2d.h"

class Clock : public cocos2d::Node {
  typedef cocos2d::Node Base;
public:
  CREATE_FUNC(Clock);
  bool init() override;
  void SetSeconds(int seconds);

private:
  void OnTimer();

private:
  int seconds_;
  Node* nums_;
};

#endif /* CLOCK_H */
