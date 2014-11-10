#ifndef LANDLORDS_LANDLORDSPLAYER_H
#define LANDLORDS_LANDLORDSPLAYER_H

#include <cstdint>
#include <vector>
#include <set>
#include <cassert>

class LandlordsPlayer {
public:
  LandlordsPlayer();
  ~LandlordsPlayer();

  void set_cards(const std::vector<int> cards) { cards_ = cards; }

  std::vector<int> cards() const { return cards_; }

  bool have_cards(const std::vector<int>& cards) const {
    std::multiset<int> own(cards_.begin(), cards_.end());
    for (auto v : cards) {
      auto iter = own.find(v);
      if (iter == own.end()) { return false; }
      own.erase(iter);
    }
    return true;
  }

  void rm_cards(const std::vector<int>& cards) {
    assert(have_cards(cards));
    for (auto v : cards) {
      auto iter = std::find(cards_.begin(), cards_.end(), v);
      assert(iter != cards_.end());
      cards_.erase(iter);
    }
  }

  bool no_cards() const { return cards_.empty(); }

private:
  std::vector<int> cards_;
};

#endif /* LANDLORDS_LANDLORDSPLAYER_H */
