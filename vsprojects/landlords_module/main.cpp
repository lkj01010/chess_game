#include <iostream>
#include <gtest/gtest.h>
#ifdef _MSC_VER
#include <vld.h>
#endif /* _MSC_VER */
#include <google/protobuf/message_lite.h>
#include <chess_server/interfaces.h>
#include "Card.h"
#include "LandlordsCard.h"
#include "LandlordsGame.h"


int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();
  return 0;
}


extern "C" {
  __declspec(dllexport) IGameLogi* CreateGame(IServerLogi* server_logi) {
    return new landlords::LandlordsGame(server_logi);
  }


  __declspec(dllexport) void DestroyGame(IGameLogi *instance) {
    delete instance;
  }
}


TEST(landlords_server_test_suite, card_test_case) {
  Card card(52);
  EXPECT_EQ(card.rank(), Rank::_BLACK_JOKER);
  EXPECT_EQ(card.suit(), Suit::INVALID);
  card.set_value(9);
  EXPECT_EQ(card.rank(), Rank::_Q);
  EXPECT_EQ(card.suit(), Suit::SPADE);
  card.set_value(27);
  EXPECT_EQ(card.rank(), Rank::_4);
  EXPECT_EQ(card.suit(), Suit::DIAMONDS);
  card.set_value(49);
  EXPECT_EQ(card.rank(), Rank::_K);
  EXPECT_EQ(card.suit(), Suit::CLUBS);
}


TEST(landlords_server_test_suite, card_type_check_test_case) {
  LandlordsCard cards;
  cards = LandlordsCard({ 5, 18, 31, 19, 45, 6 });
  EXPECT_EQ(cards.card_type(), CardType::FEIJI);
  cards = LandlordsCard({ 5, 18, 31, 19, 45, 6, 10, 11 });
  EXPECT_EQ(cards.card_type(), CardType::FEIJI);
  cards = LandlordsCard({ 5, 18, 31, 19, 45, 6, 10, 23 });
  EXPECT_EQ(cards.card_type(), CardType::FEIJI);
  cards = LandlordsCard({ 5, 18, 31, 19, 45, 6, 10, 23, 12, 25 });
  EXPECT_EQ(cards.card_type(), CardType::FEIJI);
  cards = LandlordsCard({ 5, 18, 31, 19, 45, 6, 10, 23, 12, 26 });
  EXPECT_EQ(cards.card_type(), CardType::INVALID);
  cards = LandlordsCard({ 5, 18, 31, 20, 46, 7 });
  EXPECT_EQ(cards.card_type(), CardType::INVALID);
  cards = LandlordsCard({ 3 });
  EXPECT_EQ(cards.card_type(), CardType::DANZHANG);
  cards = LandlordsCard({ 3, 16 });
  EXPECT_EQ(cards.card_type(), CardType::DUIZI);
  cards = LandlordsCard({ 3, 16, 29 });
  EXPECT_EQ(cards.card_type(), CardType::SANZHI);
  cards = LandlordsCard({ 3, 16, 29, 30 });
  EXPECT_EQ(cards.card_type(), CardType::SANZHI);
  cards = LandlordsCard({ 3, 16, 29, 30, 43 });
  EXPECT_EQ(cards.card_type(), CardType::SANZHI);
  cards = LandlordsCard({ 0, 1, 2, 3, 4, 18 });
  EXPECT_EQ(cards.card_type(), CardType::SHUNZI);
  cards = LandlordsCard({ 0, 1, 2, 3, 4, 18, 32 });
  EXPECT_EQ(cards.card_type(), CardType::SHUNZI);
  cards = LandlordsCard({ 5, 6, 7, 18, 32, 46 });
  EXPECT_EQ(cards.card_type(), CardType::LIANDUI);
  cards = LandlordsCard({ 3, 29, 42, 16 });
  EXPECT_EQ(cards.card_type(), CardType::ZHADAN);
  cards = LandlordsCard({ 52, 53 });
  EXPECT_EQ(cards.card_type(), CardType::ZHADAN);
}


TEST(landlords_server_test_suite, card_compare_test_case) {
  LandlordsCard lhs, rhs;
  lhs = LandlordsCard({ 3, 16, 29 });
  rhs = LandlordsCard({ 4, 17 });
  EXPECT_FALSE(lhs > rhs);

  lhs = LandlordsCard({ 3, 16, 29 });
  rhs = LandlordsCard({ 3, 29, 42, 16 });
  EXPECT_FALSE(lhs > rhs);
  EXPECT_TRUE(rhs > lhs);

  lhs = LandlordsCard({ 8 });
  rhs = LandlordsCard({ 7});
  EXPECT_TRUE(lhs > rhs);

  lhs = LandlordsCard({ 8, 21 });
  rhs = LandlordsCard({ 7, 33 });
  EXPECT_TRUE(lhs > rhs);

  lhs = LandlordsCard({ 8, 21, 34 });
  rhs = LandlordsCard({ 7, 33, 46 });
  EXPECT_TRUE(lhs > rhs);

  lhs = LandlordsCard({ 8, 21, 34, 4 });
  rhs = LandlordsCard({ 7, 33, 46 });
  EXPECT_FALSE(lhs > rhs);

  lhs = LandlordsCard({ 7, 8, 9, 10, 11 });
  rhs = LandlordsCard({ 6, 7, 8, 9, 10 });
  EXPECT_TRUE(lhs > rhs);

  lhs = LandlordsCard({ 6, 7, 8, 9, 10, 11 });
  rhs = LandlordsCard({ 5, 6, 7, 8, 9, 10 });
  EXPECT_TRUE(lhs > rhs);

  lhs = LandlordsCard({ 6, 7, 8, 9, 10, 11 });
  rhs = LandlordsCard({ 5, 6, 7, 8, 9 });
  EXPECT_FALSE(lhs > rhs);

  lhs = LandlordsCard({ 7, 20, 8, 21, 9, 22 });
  rhs = LandlordsCard({ 6, 19, 7, 20, 8, 21 });
  EXPECT_TRUE(lhs > rhs);

  lhs = LandlordsCard({ 7, 20, 8, 21, 9, 22, 10, 23 });
  rhs = LandlordsCard({ 6, 19, 7, 20, 8, 21 });
  EXPECT_FALSE(lhs > rhs);
}
