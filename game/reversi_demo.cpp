// Copyright 2016 iRonhead
#include <iostream>
#include <memory>
#include <string>

#include "../reversi/reversi.hpp"
#include "../uct/uct.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::shared_ptr;
using std::string;

int main() {
  UpperConfidenceTree uct(10000);

  string command;

  ReversiState* game_state = new ReversiState();
  ReversiState* temp_state = nullptr;

  temp_state = dynamic_cast<ReversiState*>(uct.SearchDemo(game_state));

  delete temp_state;
  delete game_state;
}
