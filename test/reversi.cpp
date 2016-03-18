// Copyright 2016 iRonhead
#include <algorithm>
#include <memory>
#include <vector>

#include "./catch/include/catch.hpp"
#include "../reversi/reversi.hpp"

using std::dynamic_pointer_cast;
using std::shared_ptr;
using std::sort;
using std::vector;

TEST_CASE("ReversiState static methods", "[ReversiState]") {
  SECTION("IsInvalidPosition") {
    REQUIRE(ReversiState::IsInvalidPosition(-1, -1));
    REQUIRE(ReversiState::IsInvalidPosition(0, 8));
    REQUIRE(ReversiState::IsInvalidPosition(8, 0));
    REQUIRE(ReversiState::IsInvalidPosition(8, 8));

    REQUIRE_FALSE(ReversiState::IsInvalidPosition(0, 0));
    REQUIRE_FALSE(ReversiState::IsInvalidPosition(0, 7));
    REQUIRE_FALSE(ReversiState::IsInvalidPosition(7, 0));
    REQUIRE_FALSE(ReversiState::IsInvalidPosition(7, 7));
  }
}

TEST_CASE("ReversiState", "[ReversiState]") {
  shared_ptr<ReversiState> state_base(new ReversiState());
  shared_ptr<ReversiState> state_test(new ReversiState(
    "        "
    "        "
    "        "
    "   xo   "
    "   ox   "
    "        "
    "        "
    "        ",
    ReversiState::Player::kBlack));

  SECTION("IsNormal") {
    REQUIRE(state_test->IsNormal());

    REQUIRE_FALSE(state_test->IsEnd());
  }

  SECTION("IsEnd") {
    shared_ptr<ReversiState> state_temp(new ReversiState(
      "        "
      "        "
      "        "
      "   xx   "
      "   xx   "
      "        "
      "        "
      "        ",
      ReversiState::Player::kBlack));

    REQUIRE(state_temp->IsEnd());

    REQUIRE_FALSE(state_temp->IsNormal());
  }

  SECTION("Clone") {
    shared_ptr<State> state_clone(state_test->Clone());

    shared_ptr<ReversiState> state_temp =
      dynamic_pointer_cast<ReversiState>(state_clone);

    REQUIRE(*state_test == *state_temp);
  }

  SECTION("constructor & operator==") {
    REQUIRE(*state_base == *state_test);
  }

  SECTION("IsEmptyAt") {
    REQUIRE(state_test->IsEmptyAt(0, 0));
    REQUIRE(state_test->IsEmptyAt(2, 2));

    REQUIRE_FALSE(state_test->IsEmptyAt(3, 3));
    REQUIRE_FALSE(state_test->IsEmptyAt(4, 4));
  }

  SECTION("IsBlackAt") {
    REQUIRE(state_test->IsBlackAt(3, 3));
    REQUIRE(state_test->IsBlackAt(4, 4));

    REQUIRE_FALSE(state_test->IsBlackAt(0, 0));
    REQUIRE_FALSE(state_test->IsBlackAt(3, 4));
    REQUIRE_FALSE(state_test->IsBlackAt(4, 3));
  }

  SECTION("IsWhiteAt") {
    REQUIRE(state_test->IsWhiteAt(3, 4));
    REQUIRE(state_test->IsWhiteAt(4, 3));

    REQUIRE_FALSE(state_test->IsWhiteAt(0, 0));
    REQUIRE_FALSE(state_test->IsWhiteAt(3, 3));
    REQUIRE_FALSE(state_test->IsWhiteAt(4, 4));
  }

  SECTION("PutBlackAt") {
    state_test->PutBlackAt(0, 0);
    state_test->PutBlackAt(3, 4);
    state_test->PutBlackAt(4, 3);

    REQUIRE(state_test->IsBlackAt(0, 0));
    REQUIRE(state_test->IsBlackAt(3, 4));
    REQUIRE(state_test->IsBlackAt(4, 3));
  }

  SECTION("PutWhiteAt") {
    state_test->PutWhiteAt(0, 0);
    state_test->PutWhiteAt(3, 3);
    state_test->PutWhiteAt(4, 4);

    REQUIRE(state_test->IsWhiteAt(0, 0));
    REQUIRE(state_test->IsWhiteAt(3, 3));
    REQUIRE(state_test->IsWhiteAt(4, 4));
  }

  SECTION("FlipAt") {
    state_test->FlipAt(0, 0);
    state_test->FlipAt(3, 3);
    state_test->FlipAt(3, 4);

    REQUIRE(state_test->IsEmptyAt(0, 0));
    REQUIRE(state_test->IsBlackAt(3, 4));
    REQUIRE(state_test->IsWhiteAt(3, 3));
  }

  SECTION("IsValidMoveAt") {
    REQUIRE(state_test->IsValidMoveAt(3, 2, ReversiState::Player::kWhite));
    REQUIRE(state_test->IsValidMoveAt(4, 2, ReversiState::Player::kBlack));

    REQUIRE_FALSE(
      state_test->IsValidMoveAt(0, 0, ReversiState::Player::kBlack));
    REQUIRE_FALSE(
      state_test->IsValidMoveAt(0, 0, ReversiState::Player::kWhite));
    REQUIRE_FALSE(
      state_test->IsValidMoveAt(3, 3, ReversiState::Player::kBlack));
    REQUIRE_FALSE(
      state_test->IsValidMoveAt(3, 3, ReversiState::Player::kWhite));
    REQUIRE_FALSE(
      state_test->IsValidMoveAt(3, 2, ReversiState::Player::kBlack));
    REQUIRE_FALSE(
      state_test->IsValidMoveAt(4, 2, ReversiState::Player::kWhite));
  }

  SECTION("EnumValidMoveAt") {
    auto black_moves = state_test->EnumValidMoves(ReversiState::Player::kBlack);
    auto white_moves = state_test->EnumValidMoves(ReversiState::Player::kWhite);

    vector<ReversiState::Move> valid_black_moves = {
      {4, 2}, {5, 3}, {2, 4}, {3, 5}
    };

    vector<ReversiState::Move> valid_white_moves = {
      {3, 2}, {2, 3}, {5, 4}, {4, 5}
    };

    sort(
      black_moves.begin(),
      black_moves.end(),
      [](const ReversiState::Move& ma, const ReversiState::Move& mb) -> bool {
        return ma.y < mb.y || (ma.y == mb.y && ma.x <= mb.x);
      });

    sort(
      white_moves.begin(),
      white_moves.end(),
      [](const ReversiState::Move& ma, const ReversiState::Move& mb) -> bool {
        return ma.y < mb.y || (ma.y == mb.y && ma.x <= mb.x);
      });

    REQUIRE(black_moves == valid_black_moves);
    REQUIRE(white_moves == valid_white_moves);
  }

  SECTION("MoveAt") {
    shared_ptr<ReversiState> state_moved(new ReversiState(
      "        "
      "    o   "
      "    o   "
      "   xo   "
      "   xoo  "
      "   x    "
      "        "
      "        ",
      ReversiState::Player::kBlack));

    state_test->MoveAt(4, 2);
    state_test->MoveAt(5, 4);
    state_test->MoveAt(3, 5);
    state_test->MoveAt(4, 1);

    REQUIRE(*state_test == *state_moved);
  }

  SECTION("MoveAt Without New Stone on Normal Board (not End)") {
    shared_ptr<ReversiState> state_source(new ReversiState(
      "o       "
      "x       "
      "        "
      "        "
      "        "
      "        "
      "        "
      "        ",
      ReversiState::Player::kBlack));

    shared_ptr<ReversiState> state_target(new ReversiState(
      "o       "
      "x       "
      "        "
      "        "
      "        "
      "        "
      "        "
      "        ",
      ReversiState::Player::kWhite));

    state_source->MoveAt(-1, -1);

    REQUIRE(*state_source == *state_target);
  }
}

TEST_CASE("ReversiState Expand", "[ReversiState]") {
  SECTION("With One New Move") {
    shared_ptr<ReversiState> state_source(new ReversiState(
      "o       "
      "x       "
      "        "
      "        "
      "        "
      "        "
      "        "
      "        ",
      ReversiState::Player::kWhite));

    shared_ptr<ReversiState> state_target(new ReversiState(
      "o       "
      "o       "
      "o       "
      "        "
      "        "
      "        "
      "        "
      "        ",
      ReversiState::Player::kBlack));

    auto state_expanded = dynamic_cast<ReversiState*>(state_source->Expand());

    REQUIRE(*state_expanded == *state_target);
  }

  SECTION("Without New Moves") {
    shared_ptr<ReversiState> state_source(new ReversiState(
      "o       "
      "x       "
      "        "
      "        "
      "        "
      "        "
      "        "
      "        ",
      ReversiState::Player::kBlack));

    shared_ptr<ReversiState> state_target(new ReversiState(
      "o       "
      "x       "
      "        "
      "        "
      "        "
      "        "
      "        "
      "        ",
      ReversiState::Player::kWhite));

    auto state_expanded = dynamic_cast<ReversiState*>(state_source->Expand());

    REQUIRE(*state_expanded == *state_target);
  }
}

TEST_CASE("ReversiState Count Stones", "[ReversiState]") {
  shared_ptr<ReversiState> state_00_64(new ReversiState(
    "oooooooo"
    "oooooooo"
    "oooooooo"
    "oooooooo"
    "oooooooo"
    "oooooooo"
    "oooooooo"
    "oooooooo",
    ReversiState::Player::kWhite));

  shared_ptr<ReversiState> state_32_32(new ReversiState(
    "xxxxxxxx"
    "oxxxxxxx"
    "ooxxxxxx"
    "oooxxxxx"
    "oooooxxx"
    "ooooooxx"
    "ooooooox"
    "oooooooo",
    ReversiState::Player::kWhite));

  shared_ptr<ReversiState> state_33_31(new ReversiState(
    "xxxxxxxx"
    "oxxxxxxx"
    "ooxxxxxx"
    "oooxxxxx"
    "ooooxxxx"
    "ooooooxx"
    "ooooooox"
    "oooooooo",
    ReversiState::Player::kWhite));

  SECTION("BlacksCount") {
    REQUIRE(state_00_64->BlacksCount() == 0);
    REQUIRE(state_32_32->BlacksCount() == 32);
    REQUIRE(state_33_31->BlacksCount() == 33);
  }

  SECTION("WhitesCount") {
    REQUIRE(state_00_64->WhitesCount() == 64);
    REQUIRE(state_32_32->WhitesCount() == 32);
    REQUIRE(state_33_31->WhitesCount() == 31);
  }

  SECTION("Winner") {
    REQUIRE(state_00_64->Winner() == ReversiState::Player::kWhite);
    REQUIRE(state_32_32->Winner() == ReversiState::Player::kDraw);
    REQUIRE(state_33_31->Winner() == ReversiState::Player::kBlack);
  }
}

TEST_CASE("ReversiState Simulate", "[ReversiState]") {
  SECTION("Simulate - White Win") {
    shared_ptr<ReversiState> state(new ReversiState(
      "xooooooo"
      "oooooooo"
      "oooooooo"
      "oooooooo"
      "oooooooo"
      "oooooooo"
      "oooooooo"
      "ooooooo ",
      ReversiState::Player::kBlack));

    auto winner = state->Simulate();

    REQUIRE(winner == ReversiState::Player::kWhite);
  }

  SECTION("Simulate - Draw") {
    shared_ptr<ReversiState> state(new ReversiState(
      "xxxxxxxx"
      "xxxxxxxx"
      "xxxxxxxx"
      "xxxoo xx"
      "oooooooo"
      "oooooooo"
      "oooooooo"
      "oooooooo",
      ReversiState::Player::kBlack));

    auto winner = state->Simulate();

    REQUIRE(winner == ReversiState::Player::kDraw);
  }
}

TEST_CASE("ReversiState Bugs", "[ReversiState]") {
  SECTION("IsValidMoveAt Bug") {
    shared_ptr<ReversiState> state(new ReversiState(
      "        "
      "        "
      "        "
      "       x"
      "        "
      "        "
      "       o"
      "        ",
      ReversiState::Player::kBlack));

    REQUIRE_FALSE(state->IsValidMoveAt(7, 7, ReversiState::Player::kBlack));
  }

  SECTION("MoveAt Bug") {
    shared_ptr<ReversiState> state_source(new ReversiState(
      "        "
      "   x    "
      "   xoo  "
      "   xoooo"
      "  xxxo  "
      "    x o "
      "   x    "
      "  x     ",
      ReversiState::Player::kWhite));

    shared_ptr<ReversiState> state_target(new ReversiState(
      "        "
      "   x    "
      "   xoo  "
      "   xoooo"
      "  xxoo  "
      "   ox o "
      "   x    "
      "  x     ",
      ReversiState::Player::kBlack));

    state_source->MoveAt(3, 5);

    REQUIRE(*state_source == *state_target);
  }
}
