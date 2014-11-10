#include <cassert>
#include <algorithm>
#include <map>
#include "LandlordsCard.h"
#include "CardTypeData.h"

LandlordsCard::LandlordsCard() {}


LandlordsCard::LandlordsCard(const std::vector<int> &cards) {
  SetCards(cards);
}


LandlordsCard::~LandlordsCard() {
}


CardType LandlordsCard::card_type() const {
  return card_type_data_ ? card_type_data_->card_type() : CardType::INVALID;
}


void LandlordsCard::SetCards(const std::vector<int> &cards) {
  cards_.clear();
  for (auto value : cards) {
    cards_.push_back(Card(value));
  }
  std::sort(cards_.begin(), cards_.end());
  DoAnalyse();
}


void LandlordsCard::DoAnalyse() {
  bool (LandlordsCard::*functor[])() = {
    &LandlordsCard::IsZhadan,
    &LandlordsCard::IsFeiji,
    &LandlordsCard::IsLiandui,
    &LandlordsCard::IsShunzi,
    &LandlordsCard::IsSanzhi,
    &LandlordsCard::IsDuizi,
    &LandlordsCard::IsDanzhang
  };
  for (auto func : functor) {
    card_type_data_.reset();
    if ((this->*func)()) { 
      assert(card_type_data_);
      break; 
    }
  }
}


bool LandlordsCard::IsDanzhang() { 
  if (cards_.size() != 1) {
    return false;
  }
  card_type_data_.reset(new CardTypeDataDanzhang(cards_[0].rank()));
  return true; 
}


bool LandlordsCard::IsDuizi() {
  if (cards_.size() != 2) {
    return false;
  }
  if (cards_[0].rank() == cards_[1].rank()) {
    card_type_data_.reset(new CardTypeDataDuizi(cards_[0].rank()));
    return true;
  } 
  return false;
}


bool LandlordsCard::IsSanzhi() {
  if (cards_.size() < 3 || cards_.size() > 5) { return false; }
  std::map<Rank, int> ranks;
  for (auto card : cards_) {
    ++ranks[card.rank()];
  }
  auto iter = std::find_if(ranks.begin(), ranks.end(),
    [](std::map<Rank, int>::value_type pair) {
    return pair.second == 3; 
  });
  if (iter == ranks.end()) { return false; }
  if (cards_.size() == 3) {
    card_type_data_.reset(new CardTypeDataSanzhi(iter->first,
      CardTypeDataSanzhi::SubType::_3_0));
  } else if (cards_.size() == 4) {
    card_type_data_.reset(new CardTypeDataSanzhi(iter->first,
      CardTypeDataSanzhi::SubType::_3_1));
  } else {
    auto iter = std::find_if(ranks.begin(), ranks.end(),
      [](std::map<Rank, int>::value_type pair) {
      return pair.second == 2;
    });
    if (iter == ranks.end()) { return false; }
    card_type_data_.reset(new CardTypeDataSanzhi(iter->first,
      CardTypeDataSanzhi::SubType::_3_2));
  }
  return true;
}

bool LandlordsCard::IsShunzi() {
  if (cards_.size() < 5) { return false; }
  std::vector<Rank> ranks;
  for (auto card : cards_) {
    ranks.push_back(card.rank());
  }
  std::sort(ranks.begin(), ranks.end());
  if (!is_continuous(ranks)) { return false; }
  card_type_data_.reset(new CardTypeDataShunzi(ranks[0], ranks.size()));
  return true;
}


bool LandlordsCard::IsLiandui() {
  if (cards_.size() < 6 || cards_.size() % 2) { return false; }
  std::vector<Rank> ranks;
  for (auto card : cards_) {
    ranks.push_back(card.rank());
  }
  std::sort(ranks.begin(), ranks.end());
  std::vector<Rank> ranks_single;
  for (auto i = 0u; i < ranks.size() / 2; ++i) {
    if (ranks[i * 2] != ranks[i * 2 + 1]) { return false; }
    ranks_single.push_back(ranks[i * 2]);
  }
  if (!is_continuous(ranks_single)) { return false; }
  card_type_data_.reset(new CardTypeDataLiandui(ranks_single[0], ranks_single.size()));
  return true; 
}


bool LandlordsCard::IsFeiji() { 
  if (cards_.size() < 6) { return false; }
  std::map<Rank, int> ranks;
  for (auto card : cards_) {
    ++ranks[card.rank()];
  }
  std::vector<Rank> thr;
  for (auto pair : ranks) {
    if (pair.second == 3) { thr.push_back(pair.first); }
  }
  if (thr.size() < 2) { return false; }
  std::sort(thr.begin(), thr.end());
  if (!is_continuous(thr)) { return false; }
  const int remainder = cards_.size() - thr.size() * 3;
  if (remainder == 0) {
    card_type_data_.reset(new CardTypeDataFeiji(thr[0], thr.size(),
      CardTypeDataFeiji::SubType::_3_0));
    return true;
  }
  if (remainder == thr.size()) {
    card_type_data_.reset(new CardTypeDataFeiji(thr[0], thr.size(),
      CardTypeDataFeiji::SubType::_3_1));
    return true;
  }
  if (remainder == thr.size() * 2) {
    int count = 0;
    for (auto pair : ranks) {
      if (pair.second == 2) { ++count; }
    }
    if (count == thr.size()) {
      card_type_data_.reset(new CardTypeDataFeiji(thr[0], thr.size(),
        CardTypeDataFeiji::SubType::_3_2));
      return true;
    }
  }
  return false; 
}


bool LandlordsCard::IsZhadan() {
  if (cards_.size() == 2 && cards_[0].rank() == Rank::_BLACK_JOKER
    && cards_[1].rank() == Rank::_RED_JOKER) {
    card_type_data_.reset(new CardTypeDataZhadan());
    return true;
  }
  if (cards_.size() == 4 && cards_[0].rank() == cards_[1].rank()
    && cards_[1].rank() == cards_[2].rank() 
    && cards_[2].rank() == cards_[3].rank()) {
    card_type_data_.reset(new CardTypeDataZhadan(cards_[0].rank()));
    return true;
  }
  return false;
}

bool LandlordsCard::operator>(const LandlordsCard& rhs) const {
  assert(card_type_data_ && rhs.card_type_data_);
  assert(card_type_data_->card_type() != CardType::INVALID
    && rhs.card_type_data_->card_type() != CardType::INVALID);
  if (card_type_data_->card_type() == rhs.card_type_data_->card_type()) {
    return *card_type_data_ > *rhs.card_type_data_;
  } else {
    return card_type() == CardType::ZHADAN;
  }
  return false;
}

LandlordsCard& LandlordsCard::operator=(LandlordsCard &&rhs) {
  cards_ = std::move(rhs.cards_);
  card_type_data_ = std::move(rhs.card_type_data_);
  return *this;
}

std::ostream& operator<<(std::ostream &os, const LandlordsCard &cards) {
  os << (cards.card_type_data_ ? cards.card_type_data_->ToString() : "非法") << ": [" << cards.ToString() << "]";
  return os;
}
