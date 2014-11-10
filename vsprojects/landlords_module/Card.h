#ifndef LANDLORDS_SERVER_CARD_H
#define LANDLORDS_SERVER_CARD_H

#include <string>
#include <cassert>

enum class Suit {
  SPADE = 1,
  HEARTS,
  DIAMONDS,
  CLUBS,
  INVALID
};


enum class Rank {
  _3,
  _4,
  _5,
  _6,
  _7,
  _8,
  _9,
  _10,
  _J,
  _Q,
  _K,
  _A,
  _2,
  _BLACK_JOKER,
  _RED_JOKER,
  INVALID
};


inline std::string ToString(const Suit &suit) {
  switch (suit) {
  case Suit::SPADE: return "黑桃";
  case Suit::HEARTS: return "红桃";
  case Suit::DIAMONDS: return "方块";
  case Suit::CLUBS: return "梅花";
  default: return "";
  }
}

inline std::string ToString(const Rank &suit) {
  switch (suit) {
  case Rank::_3: return "3";
  case Rank::_4: return "4";
  case Rank::_5: return "5";
  case Rank::_6: return "6";
  case Rank::_7: return "7";
  case Rank::_8: return "8";
  case Rank::_9: return "9";
  case Rank::_10: return "10";
  case Rank::_J: return "J";
  case Rank::_Q: return "Q";
  case Rank::_K: return "K";
  case Rank::_A: return "A";
  case Rank::_2: return "2";
  case Rank::_BLACK_JOKER: return "小王";
  case Rank::_RED_JOKER: return "大王";
  default: return "";
  }
}


class Card {
public:
  /**
   * 一共有54张牌：
   *  0~12 黑桃3,4,5,6,7,8,9,10,J,Q,K,A,2
   * 13~25 红桃3,4,5,6,7,8,9,10,J,Q,K,A,2
   * 26~38 方块3,4,5,6,7,8,9,10,J,Q,K,A,2
   * 39~51 梅花3,4,5,6,7,8,9,10,J,Q,K,A,2
   * 52 小王
   * 53 大王
   */
  Card(int value) : value_(value) {
    assert(value >= 0 && value_ <= 53);
  }

  int value() const { return value_; }

  void set_value(int value) { value_ = value; }
  
  Suit suit() const {
    if (value_ < 13) { return Suit::SPADE; }
    if (value_ < 26) { return Suit::HEARTS; }
    if (value_ < 39) { return Suit::DIAMONDS; }
    if (value_ < 52) { return Suit::CLUBS; }
    return Suit::INVALID;
  }

  Rank rank() const {
    if (value_ == 52) { return Rank::_BLACK_JOKER; }
    if (value_ == 53) { return Rank::_RED_JOKER; }
    return static_cast<Rank>(value_ % 13);
  }

  bool operator<(const Card &rhs) const {
    return value_ < rhs.value_;
  }

  std::string ToString() const {
    return ::ToString(suit()) + ::ToString(rank());
  }

private:
  int value_;
};

#endif /* LANDLORDS_SERVER_CARD_H */
