// Copyright 2016 iRonhead
#include <algorithm>
#include <cassert>
#include <limits>
#include "state.hpp"

using std::max_element;
using std::numeric_limits;

//------------------------------------------------------------------------------
State::State() : count_wins_(0.0f), count_visits_(0.0f), parent_(nullptr),
    value_(numeric_limits<float>::max()), type_(State::Type::kUnknown) {
}

//------------------------------------------------------------------------------
State::~State() {
  // Assume everyone is in heap.
  for (auto child : this->children_) {
    delete child;
  }
}

//------------------------------------------------------------------------------
bool State::IsNormal() { return true; }

//------------------------------------------------------------------------------
bool State::IsEnd() { return true; }

//------------------------------------------------------------------------------
State* State::BestMove() const {
  auto best = max_element(
    this->children_.begin(),
    this->children_.end(),
    [](const State* a, const State* b) -> bool {
      return a->count_visits_ < b->count_visits_;
    });

  return (*best)->Clone();
}

//------------------------------------------------------------------------------
State* State::Clone() const {
  return nullptr;
}

//------------------------------------------------------------------------------
State* State::Select() {
  // If there are no more moves, return this.
  // If there are no children, return this to expand.
  if (this->IsEnd() || this->children_.empty()) { return this; }

  auto selected = max_element(
    this->children_.begin(),
    this->children_.end(),
    [](const State* a, const State* b) -> bool {
      return a->value_ < b->value_;
    });

  return (*selected)->Select();
}

//------------------------------------------------------------------------------
State* State::Expand() {
  // Expand only when this is normal (not an end).
  assert(this->IsNormal());

  // Expand only when there are no children.
  assert(this->children_.empty());

  return nullptr;
}

//------------------------------------------------------------------------------
int32_t State::Simulate() {
  return -1;
}

//------------------------------------------------------------------------------
void State::Backpropagate(int32_t winner) {
}

//------------------------------------------------------------------------------
void State::Inspect() const {
}
