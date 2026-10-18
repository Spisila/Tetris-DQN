#pragma once

constexpr size_t STATE_SIZE = 67;
constexpr size_t ACTION_SPACE_SIZE = 40;
constexpr size_t QUEUE_SIZE = 5;

struct Position
{
  int x;
  int y;

  Position(int _x, int _y) : x(_x), y(_y) {}
};


inline int formatPiece(int piece)
{
  switch (piece)
  {
  case 0:
    return 0;
  case 4:
    return 1;
  case 8:
    return 2;
  case 12:
    return 3;
  case 16:
    return 4;
  case 20:
    return 5;
  case 24:
    return 6;
  default:
    return -1;
  }
}