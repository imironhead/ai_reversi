// Copyright 2016 iRonhead
#ifndef REVERSI_STATE_H__
#define REVERSI_STATE_H__

#include <cstdint>
#include <vector>

class State {
 public:
  enum class Type { kUnknown, kNormal, kEnd };

 public:
  State();
  virtual ~State() = 0;

  virtual bool IsNormal();
  virtual bool IsEnd();
  virtual State* BestMove() const;
  virtual State* Clone() const;
  virtual State* Select();
  virtual State* Expand();
  virtual int32_t Simulate();
  virtual void Backpropagate(int32_t winner);
  virtual void Inspect() const;

 protected:
  Type                type_;
  float               value_;
  float               count_wins_;
  float               count_visits_;
  State*              parent_;
  std::vector<State*> children_;
};

#endif  // REVERSI_STATE_H__
