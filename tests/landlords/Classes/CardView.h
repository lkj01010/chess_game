#ifndef CARDVIEW_H
#define CARDVIEW_H

#include "ui/CocosGUI.h"

class CardView : public cocos2d::ui::ImageView {
public:
  static CardView* create(const std::string& imageFileName,
                          TextureResType texType = TextureResType::LOCAL) {
    CardView *widget = new CardView;
    if (widget && widget->init(imageFileName, texType)) {
      widget->autorelease();
      return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
  }

  bool selected = false;
  int value = -1;
};

#endif /* CARDVIEW_H */
