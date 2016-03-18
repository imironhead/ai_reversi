// Copyright 2016 iRonhead
#ifndef REVERSI_UCT_H__
#define REVERSI_UCT_H__

#include <cstdint>
#include "state.hpp"

class UpperConfidenceTree {
 public:
  explicit UpperConfidenceTree(int32_t count_round);

  State* Search(State* root) const;
  State* SearchDemo(State* root) const;

 private:
  int32_t count_round_;
};

#endif  // REVERSI_UCT_H__
