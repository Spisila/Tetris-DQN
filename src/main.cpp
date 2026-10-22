#include <raylib.h>

#include <iostream>
#include <random>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <cstdlib>

#include <vector>

#include <game.hpp>
#include <game_renderer.hpp>

int main()
{

  Game main_game{1, 22};
  GameRenderer renderer{main_game};

  Board &board = main_game.getBoard();
  PieceQueue &queue = main_game.getPieceQueue();

  renderer.initWindow();

  while (!WindowShouldClose())
  {
    renderer.drawLoop();
    // main_game.tickGravity();

    if (IsKeyPressed(KEY_LEFT))
    {
      board.moveTetromino(MovementDirection::LEFT);
    }
    else if (IsKeyPressed(KEY_RIGHT))
    {
      board.moveTetromino(MovementDirection::RIGHT);
    }
    else if (IsKeyPressed(KEY_DOWN))
    {
      board.moveTetromino(MovementDirection::DOWN);
    }

    if (IsKeyPressed(KEY_Z))
    {
      board.rotateTetromino(Rotation::COUNTER_CLOCKWISE);
    }
    else if (IsKeyPressed(KEY_X))
    {
      board.rotateTetromino(Rotation::CLOCKWISE);
    }
    else if (IsKeyPressed(KEY_A))
    {
      board.rotateTetromino(Rotation::ONE_EIGHTY);
    }

    if (IsKeyPressed(KEY_C) && main_game.hold_used == false)
    {
      main_game.holdCurrentPiece();
      main_game.hold_used = true;
    }

    if (IsKeyPressed(KEY_SPACE))
    {
      main_game.hardDrop();
    }

    int cleared_lines = main_game.getBoard().clearLines();

    main_game.increaseScore(cleared_lines);
  }

  CloseWindow();
  return 0;
}