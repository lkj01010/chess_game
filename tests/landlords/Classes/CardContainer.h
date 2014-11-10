#ifndef CARDCONTAINER_H
#define CARDCONTAINER_H

#include <vector>
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "CardView.h"

class CardContainer : public cocos2d::Node {
  typedef cocos2d::Node Base;
public:
  static CardContainer* CreatefHorizontal(float margin, float offset, bool touchable) {
    return create(margin, offset, touchable, true);
  }

  static CardContainer* CreatefVertical(float margin, float offset, bool touchable) {
    return create(margin, offset, touchable, false);
  }

  CardContainer();
  ~CardContainer();
  virtual bool init() override;
  void SetCards(const std::vector<int>& cards);
  void PushCard(int card);
  void InsertCard(int pos, int card);
  int GetCardSize() const;
  std::vector<int> GetCards() const;
  std::vector<int> GetSelectedCards() const;
  void Select(int pos);
  void Select(const std::vector<int>& pos);
  void Deselect();
  void Deselect(int pos);
  void Deselect(const std::vector<int>& pos);
  void Remove(const std::vector<int>& cards);

private:
  static CardContainer* create(float margin, float offset, bool touchable, bool is_horizontal) {
    CardContainer *object = new CardContainer();
    if (!object) { return nullptr; }
    object->margin_ = margin;
    object->offset_ = offset;
    object->touchable_ = touchable;
    object->is_horizontal_ = is_horizontal;
    if (object->init()) { 
      object->autorelease();
      return object; 
    }
    delete object;
    return nullptr;
  }

  void OnTouch(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
  void AdjustCardPosition(bool animate);

private:
  bool is_horizontal_;
  float margin_, offset_;
  bool touchable_;
  bool batch_insert_;
  std::vector<CardView*> views_;
};

#endif /* CARDCONTAINER_H */
