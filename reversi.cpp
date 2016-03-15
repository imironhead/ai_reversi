// Copyright 2016 iRonhead
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "reversi.hpp"

using std::cout;
using std::dynamic_pointer_cast;
using std::endl;
using std::log;
using std::rand;
using std::shared_ptr;
using std::sqrt;
using std::string;
using std::vector;

//------------------------------------------------------------------------------
const int32_t ReversiState::kDirectionsX[8] = {-1, +0, +1, -1, +1, -1, +0, +1};
const int32_t ReversiState::kDirectionsY[8] = {-1, -1, -1, +0, +0, +1, +1, +1};

//------------------------------------------------------------------------------
bool ReversiState::IsInvalidPosition(int32_t x, int32_t y) {
  return x < 0 || x > 7 || y < 0 || y > 7;
}

//------------------------------------------------------------------------------
ReversiState::ReversiState() :
    blacks_(0x0000001008000000), whites_(0x0000000810000000),
    player_(Player::kBlack) {
}

//------------------------------------------------------------------------------
ReversiState::ReversiState(const char* stones, Player player) :
    blacks_(0), whites_(0), player_(player) {
  uint64_t f = 1;

  for (auto y = 0; y < 8; ++y) {
    for (auto x = 0; x < 8; ++x) {
      if (*stones == 'o') {
        this->whites_ |= f;
      } else if (*stones == 'x') {
        this->blacks_ |= f;
      }

      stones += 1;
      f <<= 1;
    }
  }
}

//------------------------------------------------------------------------------
ReversiState::~ReversiState() {}

//------------------------------------------------------------------------------
bool ReversiState::IsNormal() {
  if (this->type_ == State::Type::kUnknown) {
    return !this->IsEnd();
  } else {
    return this->type_ == State::Type::kNormal;
  }
}

//------------------------------------------------------------------------------
bool ReversiState::IsEnd() {
  if (this->type_ == State::Type::kUnknown) {
    auto moves = this->EnumValidMoves(ReversiState::Player::kBlack);

    if (moves.empty()) {
      moves = this->EnumValidMoves(ReversiState::Player::kWhite);
    }

    this->type_ =
      moves.empty() ? State::Type::kEnd : State::Type::kNormal;
  }

  return this->type_ == State::Type::kEnd;
}

//------------------------------------------------------------------------------
State* ReversiState::Clone() const {
  ReversiState* state = new ReversiState();

  state->blacks_ = this->blacks_;
  state->whites_ = this->whites_;
  state->player_ = this->player_;

  return state;
}

//------------------------------------------------------------------------------
State* ReversiState::Expand() {
  // Expand only when this is normal (not an end).
  assert(this->IsNormal());

  // Expand only when there are no children.
  assert(this->children_.empty());

  auto moves = this->EnumValidMoves(this->player_);

  if (moves.empty()) {
    // This state is not an end and there is no move for this->player_.
    // The player of this state should be flipped then.
    auto state = dynamic_cast<ReversiState*>(this->Clone());

    state->parent_ = this;

    state->MoveAt(-1, -1);

    this->children_.emplace_back(state);
  } else {
    for (auto move : moves) {
      auto state = dynamic_cast<ReversiState*>(this->Clone());

      state->parent_ = this;

      state->MoveAt(move.x, move.y);

      this->children_.emplace_back(state);
    }
  }

  return this->children_.front();
}

//------------------------------------------------------------------------------
int32_t ReversiState::Simulate() {
  shared_ptr<State> state_clone(this->Clone());
  auto state = dynamic_pointer_cast<ReversiState>(state_clone);

  while (state->IsNormal()) {
    auto moves = state->EnumValidMoves(state->player_);

    if (moves.empty()) {
      state->MoveAt(-1, -1);
    } else {
      auto move = moves[rand() % moves.size()];

      state->MoveAt(move.x, move.y);
    }
  }

  return state->Winner();
}

//------------------------------------------------------------------------------
void ReversiState::Backpropagate(int32_t winner) {
  this->count_visits_ += 1.0f;
  this->count_wins_ += (this->player_ == winner ? 0.0f : 1.0f);

  if (this->parent_ != nullptr) {
    auto parent = dynamic_cast<ReversiState*>(this->parent_);

    float exploitation = this->count_wins_ / this->count_visits_;

    float exploration = sqrt(
      2.0f * log(parent->count_visits_ + 1.0f) / this->count_visits_);

    this->value_ = exploitation + exploration;

    parent->Backpropagate(winner);
  }
}

//------------------------------------------------------------------------------
void ReversiState::Inspect() const {
  const auto blacks = this->blacks_;
  const auto whites = this->whites_;

  auto f = 1ull;

  cout << "   0 1 2 3 4 5 6 7 " << endl;
  cout << "  \u250c\u2500\u252c\u2500\u252c\u2500\u252c\u2500\u252c\u2500\u252c"
          "\u2500\u252c\u2500\u252c\u2500\u2510" << endl;

  for (auto y = 0; y < 8; ++y) {
    cout << static_cast<char>('A' + y) << " \u2502";

    for (auto x = 0; x < 8; ++x) {
      if ((blacks & f) != 0) {
        cout << "\u25cf\u2502";
      } else if ((whites & f) != 0) {
        cout << "\u25cb\u2502";
      } else {
        cout << " \u2502";
      }

      f <<= 1;
    }

    cout << endl;

    if (y < 7) {
      cout << "  \u251c\u2500\u253c\u2500\u253c\u2500\u253c\u2500\u253c\u2500"
              "\u253c\u2500\u253c\u2500\u253c\u2500\u2524" << endl;
    } else {
      cout << "  \u2514\u2500\u2534\u2500\u2534\u2500\u2534\u2500\u2534\u2500"
              "\u2534\u2500\u2534\u2500\u2534\u2500\u2518" << endl;
    }
  }

  cout << "  \u25cf: " << this->BlacksCount();
  cout << "  \u25cb: " << this->WhitesCount();
  cout << endl;
}

//------------------------------------------------------------------------------
bool ReversiState::operator==(const ReversiState& that) const {
  return
    this->blacks_ == that.blacks_ &&
    this->whites_ == that.whites_ &&
    this->player_ == that.player_;
}

//------------------------------------------------------------------------------
bool ReversiState::IsEmptyAt(int32_t x, int32_t y) const {
  const auto f = (1ull << (8 * y + x));

  return 0ull == (this->blacks_ & f) && 0ull == (this->whites_ & f);
}

//------------------------------------------------------------------------------
bool ReversiState::IsBlackAt(int32_t x, int32_t y) const {
  const auto f = (1ull << (8 * y + x));

  return 0ull != (this->blacks_ & f);
}

//------------------------------------------------------------------------------
bool ReversiState::IsWhiteAt(int32_t x, int32_t y) const {
  const auto f = (1ull << (8 * y + x));

  return 0ull != (this->whites_ & f);
}

//------------------------------------------------------------------------------
bool ReversiState::IsValidMoveAt(int32_t x, int32_t y, Player c) const {
  if (ReversiState::IsInvalidPosition(x, y)) { return false; }
  if (!this->IsEmptyAt(x, y)) { return false; }

  auto fn_is_same =
    (c == Player::kBlack) ? &ReversiState::IsBlackAt : &ReversiState::IsWhiteAt;
  auto fn_is_diff =
    (c == Player::kBlack) ? &ReversiState::IsWhiteAt : &ReversiState::IsBlackAt;

  int32_t u, v, du, dv;

  for (auto i = 0; i < 8; ++i) {
    du = ReversiState::kDirectionsX[i];
    dv = ReversiState::kDirectionsY[i];

    u = x + du;
    v = y + dv;

    while ((this->*fn_is_diff)(u, v)) {
      u += du;
      v += dv;

      if (ReversiState::IsInvalidPosition(u, v)) { break; }
      if (this->IsEmptyAt(u, v)) { break; }
      if ((this->*fn_is_same)(u, v)) { return true; }
    }
  }

  return false;
}

//------------------------------------------------------------------------------
void ReversiState::PutBlackAt(int32_t x, int32_t y) {
  const auto f = (1ull << (8 * y + x));

  this->blacks_ |= f;
  this->whites_ &= ~f;
}

//------------------------------------------------------------------------------
void ReversiState::PutWhiteAt(int32_t x, int32_t y) {
  const auto f = (1ull << (8 * y + x));

  this->blacks_ &= ~f;
  this->whites_ |= f;
}

//------------------------------------------------------------------------------
void ReversiState::FlipAt(int32_t x, int32_t y) {
  const auto f = (1ull << (8 * y + x));

  if ((this->blacks_ & f) != 0ull) {
    this->blacks_ &= ~f;
    this->whites_ |= f;
  } else if ((this->whites_ & f) != 0ull) {
    this->blacks_ |= f;
    this->whites_ &= ~f;
  }
}

//------------------------------------------------------------------------------
void ReversiState::MoveAt(int32_t x, int32_t y) {
  if (ReversiState::IsInvalidPosition(x, y)) {
    // flip player only.
  } else {
    assert(this->IsEmptyAt(x, y));

    bool moved = false;

    auto fn_is_same = (this->player_ == Player::kBlack)
      ? &ReversiState::IsBlackAt : &ReversiState::IsWhiteAt;
    auto fn_is_diff = (this->player_ == Player::kBlack)
      ? &ReversiState::IsWhiteAt : &ReversiState::IsBlackAt;

    int32_t u, v, du, dv;

    for (auto i = 0; i < 8; ++i) {
      du = ReversiState::kDirectionsX[i];
      dv = ReversiState::kDirectionsY[i];

      u = x + du;
      v = y + dv;

      while ((this->*fn_is_diff)(u, v)) {
        u += du;
        v += dv;

        if (ReversiState::IsInvalidPosition(u, v)) { break; }
        if (this->IsEmptyAt(u, v)) { break; }

        if ((this->*fn_is_same)(u, v)) {
          while (true) {
            u -= du;
            v -= dv;

            if (u == x && v == y) { break; }

            this->FlipAt(u, v);
          }

          moved = true;

          break;
        }
      }
    }

    if (this->player_ == ReversiState::Player::kBlack) {
      this->PutBlackAt(x, y);
    } else {
      this->PutWhiteAt(x, y);
    }

    this->type_ = State::Type::kUnknown;

    assert(moved);
  }

  this->player_ = (this->player_ == ReversiState::Player::kBlack
                  ? ReversiState::Player::kWhite
                  : ReversiState::Player::kBlack);
}

//------------------------------------------------------------------------------
int32_t ReversiState::BlacksCount() const {
  auto temp = this->blacks_;
  auto count = 0;

  while (temp != 0) {
    if ((temp & 1) != 0) { ++count; }

    temp >>= 1;
  }

  return count;
}

//------------------------------------------------------------------------------
int32_t ReversiState::WhitesCount() const {
  auto temp = this->whites_;
  auto count = 0;

  while (temp != 0) {
    if ((temp & 1) != 0) { ++count; }

    temp >>= 1;
  }

  return count;
}

//------------------------------------------------------------------------------
ReversiState::Player ReversiState::CurrentPlayer() const {
  return this->player_;
}

//------------------------------------------------------------------------------
ReversiState::Player ReversiState::Winner() const {
  auto blacks_count = this->BlacksCount();
  auto whites_count = this->WhitesCount();

  if (blacks_count == whites_count) {
    return ReversiState::Player::kDraw;
  } else if (blacks_count > whites_count) {
    return ReversiState::Player::kBlack;
  } else {
    return ReversiState::Player::kWhite;
  }
}

//------------------------------------------------------------------------------
vector<ReversiState::Move> ReversiState::EnumValidMoves(Player c) const {
  vector<ReversiState::Move> moves;

  for (auto y = 0; y < 8; ++y) {
    for (auto x = 0; x < 8; ++x) {
      if (this->IsValidMoveAt(x, y, c)) { moves.emplace_back(x, y); }
    }
  }

  return moves;
}
