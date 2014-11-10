#ifndef LANDLORDS_LANDLORDSCARD_H
#define LANDLORDS_LANDLORDSCARD_H

#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>
#include "Card.h"

class CardTypeData;

enum CardType {
  DANZHANG, ///< 单张
  DUIZI,    ///< 对子
  SANZHI,   ///< 三只
  SHUNZI,   ///< 顺子
  LIANDUI,  ///< 连对
  FEIJI,    ///< 飞机
  ZHADAN,   ///< 炸弹
  INVALID
};


class LandlordsCard {
public:
  LandlordsCard();
  LandlordsCard(const std::vector<int> &cards);
  ~LandlordsCard();

  CardType card_type() const;

  void SetCards(const std::vector<int> &cards);

  bool operator>(const LandlordsCard& rhs) const;

  LandlordsCard& operator=(LandlordsCard &&rhs);
  
  std::string ToString() const {
    std::string desc;
    for (auto card : cards_) {
      if (!desc.empty()) { desc += ", "; }
      desc += card.ToString();
    }
    return desc;
  }

private:
  void DoAnalyse();
  bool IsDanzhang();
  bool IsDuizi();
  bool IsSanzhi();
  bool IsShunzi();
  bool IsLiandui();
  bool IsFeiji();
  bool IsZhadan();

  bool is_continuous(std::vector<Rank> &ranks) const {
    std::sort(ranks.begin(), ranks.end());
    std::vector<Rank> range = { Rank::_3, Rank::_4, Rank::_5, Rank::_6, Rank::_7,
      Rank::_8, Rank::_9, Rank::_10, Rank::_J, Rank::_Q, Rank::_K, Rank::_A };
    int start_index = -1;
    for (auto i = 0u; i < range.size(); ++i) {
      if (range[i] == ranks[0]) {
        start_index = i;
        break;
      }
    }
    if (-1 == start_index) { return false; }
    if (start_index + ranks.size() > range.size()) { return false; }
    for (auto i = 0u; i < ranks.size(); ++i) {
      if (ranks[i] != range[start_index + i]) {
        return false;
      }
    }
    return true;
  }

private:
  std::vector<Card> cards_;
  std::unique_ptr<CardTypeData> card_type_data_;

  friend std::ostream& operator<<(std::ostream &os, const LandlordsCard &cards);
};


std::ostream& operator<<(std::ostream &os, const LandlordsCard &cards);

#endif /* LANDLORDS_LANDLORDSCARD_H */
