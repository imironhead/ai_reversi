// Copyright 2016 iRonhead
#ifndef REVERSI_REVERSI_H__
#define REVERSI_REVERSI_H__

#include <cstdint>
#include <vector>
#include "../uct/state.hpp"

class ReversiState : public State {
 public:
  enum Player { kBlack, kWhite, kDraw };

  struct Move {
    int32_t x;
    int32_t y;

    Move(int32_t x_, int32_t y_) : x(x_), y(y_) {}

    bool operator==(const Move& that) const {
      return this->x == that.x && this->y == that.y;
    }
  };

 public:
  // Check if x & y is on the board.
  static bool IsInvalidPosition(int32_t x, int32_t y);

  // Default, the first move is black.
  // "        "
  // "        "
  // "        "
  // "   xo   "
  // "   ox   "
  // "        "
  // "        "
  // "        "
  ReversiState();

  // Construct with string board.
  // ReversiState(
  //   "        "
  //   "        "
  //   "        "
  //   "   xo   "
  //   "   ox   "
  //   "        "
  //   "        "
  //   "        ",
  //   Player::kBlack);
  // equals to default constructor.
  ReversiState(const char* stones, Player player);
  ~ReversiState() override;

  bool IsNormal() override;
  bool IsEnd() override;
  State* Clone() const override;
  State* Expand() override;
  int32_t Simulate() override;
  void Backpropagate(int32_t winner) override;
  void Inspect() const override;
  void InspectValue() const override;

  // Compare 2 ReversiStates with their stones and current player.
  bool operator==(const ReversiState& that) const;

  bool IsEmptyAt(int32_t x, int32_t y) const;
  bool IsBlackAt(int32_t x, int32_t y) const;
  bool IsWhiteAt(int32_t x, int32_t y) const;
  bool IsValidMoveAt(int32_t x, int32_t y, Player c) const;
  void PutBlackAt(int32_t x, int32_t y);
  void PutWhiteAt(int32_t x, int32_t y);
  void FlipAt(int32_t x, int32_t y);
  void MoveAt(int32_t x, int32_t y);
  int32_t BlacksCount() const;
  int32_t WhitesCount() const;
  Player CurrentPlayer() const;
  Player Winner() const;
  std::vector<Move> EnumValidMoves(Player c) const;

 private:
  static const int32_t  kDirectionsX[8];
  static const int32_t  kDirectionsY[8];

  uint64_t  blacks_;
  uint64_t  whites_;
  Player    player_;
};

#endif  // REVERSI_REVERSI_H__
