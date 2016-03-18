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

  game_state->Inspect();

  while (true) {
    if (game_state->CurrentPlayer() == ReversiState::Player::kBlack) {
      cout << "Your Turn..." << endl;

      auto moves = game_state->EnumValidMoves(ReversiState::Player::kBlack);

      if (moves.empty()) {
        game_state->MoveAt(-1, -1);
      } else {
        cin >> command;

        if ((command.length() != 2) ||
            (command[0] < '0' || command[0] > '9') ||
            (command[1] < 'A' || command[1] > 'H')) {
          cout << " - Invalid Input -\n";
          continue;
        }

        const int32_t x = command[0] - '0';
        const int32_t y = command[1] - 'A';

        if (game_state->IsValidMoveAt(x, y, game_state->CurrentPlayer())) {
          game_state->MoveAt(x, y);
        } else {
          cout << " - Invalid Move -\n";
          continue;
        }
      }
    } else {
      cout << "(~_~)...thinking..." << endl;

      temp_state = dynamic_cast<ReversiState*>(uct.Search(game_state));

      delete game_state;

      game_state = temp_state;
    }

    game_state->Inspect();

    if (game_state->IsEnd()) {
      switch (game_state->Winner()) {
        case ReversiState::Player::kBlack: {
          cout << "   - Winner: \u25cf -\n";
          cout << "     <(/_\\)>\n\n";
        } break;

        case ReversiState::Player::kWhite: {
          cout << "   - Winner: \u25cb -\n";
          cout << "     <(^_^)@\n\n";
        } break;

        default: {
          cout << "     - DRAW -\n\n";
        } break;
      }

      break;
    }
  }

  delete game_state;
}
