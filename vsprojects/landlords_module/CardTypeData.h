#include "LandlordsCard.h"

class CardTypeData {
public:
  CardTypeData() {
    card_type_ = CardType::INVALID;
  }

  virtual ~CardTypeData() {}

  CardType card_type() const { return card_type_; }
  virtual std::string ToString() = 0;

  bool operator>(const CardTypeData &rhs) const;

private:
  template<typename T>
  bool is_great(const CardTypeData &rhs) const;

protected:
  CardType card_type_;
};


class CardTypeDataDanzhang : public CardTypeData {
public:
  CardTypeDataDanzhang(Rank rank) {
    card_type_ = CardType::DANZHANG;
    rank_ = rank;
  }

  std::string ToString() override {
    return "单张";
  }

  bool operator>(const CardTypeDataDanzhang &rhs) const {
    assert(rhs.card_type() == CardType::DANZHANG);
    return rank_ > rhs.rank_;
  }

private:
  Rank rank_;
};


class CardTypeDataDuizi : public CardTypeData {
public:
  CardTypeDataDuizi(Rank rank) {
    card_type_ = CardType::DUIZI;
    rank_ = rank;
  }

  std::string ToString() override {
    return "对子";
  }

  bool operator>(const CardTypeDataDuizi &rhs) const {
    assert(rhs.card_type() == CardType::DUIZI);
    return rank_ > rhs.rank_;
  }

private:
  Rank rank_;
};


class CardTypeDataSanzhi : public CardTypeData {
public:
  enum class SubType {
    _3_0, ///< 三个
    _3_1, ///< 三带一
    _3_2  ///< 三带一对
  };

  CardTypeDataSanzhi(Rank rank, SubType sub_type) {
    card_type_ = CardType::SANZHI;
    rank_ = rank;
    sub_type_ = sub_type;
  }

  std::string ToString() override {
    switch (sub_type_) {
    case SubType::_3_0: return "三只";
    case SubType::_3_1: return "三带一";
    case SubType::_3_2: return "三带一对";
    }
    assert(0);
    return "";
  }

  bool operator>(const CardTypeDataSanzhi& rhs) const {
    if (sub_type_ != rhs.sub_type_) { return false; }
    return rank_ > rhs.rank_;
  }

private:
  Rank rank_;
  SubType sub_type_;
};


class CardTypeDataShunzi : public CardTypeData {
public:
  CardTypeDataShunzi(Rank start, int count) {
    card_type_ = CardType::SHUNZI;
    start_ = start;
    count_ = count;
  }

  std::string ToString() override {
    return "顺子";
  }

  bool operator>(const CardTypeDataShunzi &rhs) const {
    if (count_ != rhs.count_) { return false; }
    return start_ > rhs.start_;
  }

private:
  Rank start_;
  int count_;
};


class CardTypeDataLiandui : public CardTypeData {
public:
  CardTypeDataLiandui(Rank start, int count) {
    card_type_ = CardType::LIANDUI;
    start_ = start;
    count_ = count;
  }

  std::string ToString() override {
    return "连对";
  }

  bool operator>(const CardTypeDataLiandui& rhs) const {
    if (count_ != rhs.count_) { return false; }
    return start_ > rhs.start_;
  }

private:
  Rank start_;
  int count_;
};


class CardTypeDataFeiji : public CardTypeData {
public:
  enum class SubType {
    _3_0, ///< 不带
    _3_1, ///< 带单张
    _3_2  ///< 带对子
  };

  std::string ToString() override {
    return "飞机";
  }

  CardTypeDataFeiji(Rank start, int count, SubType sub_type) {
    card_type_ = CardType::FEIJI;
    start_ = start;
    count_ = count;
    sub_type_ = sub_type;
  }

  bool operator>(const CardTypeDataFeiji& rhs) const {
    if (sub_type_ != rhs.sub_type_) { return false; }
    if (count_ != rhs.count_) { return false; }
    return start_ > rhs.start_;
  }

private:
  Rank start_;
  int count_;
  SubType sub_type_;
};


class CardTypeDataZhadan : public CardTypeData {
public:
  CardTypeDataZhadan(Rank rank = Rank::INVALID) {
    card_type_ = CardType::ZHADAN;
    rank_ = rank;
  }

  std::string ToString() override {
    return "炸弹";
  }

  bool operator>(const CardTypeDataZhadan& rhs) const {
    if (rank_ == Rank::INVALID && rhs.rank_ != Rank::INVALID) { return true; }
    if (rank_ != Rank::INVALID && rhs.rank_ != Rank::INVALID) {
      return rank_ > rhs.rank_;
    }
    return false;
  }

private:
  Rank rank_;
};

template<typename T>
inline bool CardTypeData::is_great(const CardTypeData &rhs) const {
  return dynamic_cast<const T&>(*this) > dynamic_cast<const T&>(rhs);
}

inline bool CardTypeData::operator>(const CardTypeData &rhs) const {
  assert(card_type_ == rhs.card_type_);
  switch (card_type_) {
  case CardType::DANZHANG: return is_great<CardTypeDataDanzhang>(rhs);
  case CardType::DUIZI: return is_great<CardTypeDataDuizi>(rhs);
  case CardType::SANZHI: return is_great<CardTypeDataSanzhi>(rhs);
  case CardType::SHUNZI: return is_great<CardTypeDataShunzi>(rhs);
  case CardType::LIANDUI: return is_great<CardTypeDataLiandui>(rhs);
  case CardType::FEIJI: return is_great<CardTypeDataFeiji>(rhs);
  case CardType::ZHADAN: return is_great<CardTypeDataZhadan>(rhs);
  }
  return false;
}
