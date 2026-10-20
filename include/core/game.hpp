#pragma once

#include <raylib.h>

#include <vector>
#include <array>
#include <unordered_map>

#include <board.hpp>
#include <piece_queue.hpp>
#include <types.hpp>

#include <shared.hpp>

constexpr int piece_queue_size = 5;

class Game
{
private:
  int id;

  bool gravity_on = false;
  int gravity_counter = 0;
  int gravity_ticks = 30;

  Board game_board{};
  PieceQueue piece_queue{};

  int lines_cleared = 0;
  int lines_cleared_last = 0;

  unsigned int single_line_clears = 0;
  unsigned int double_line_clears = 0;
  unsigned int triple_line_clears = 0;
  unsigned int tetris_line_clears = 0;

  float last_landing_height = 0.0f;
  int last_eroded_piece_cells = 0;

  int score = 0;

  std::array<Position, 4> last_piece_placed_positions{
      Position(0, 0),
      Position(0, 0),
      Position(0, 0),
      Position(0, 0)};

public:
  Actions next_action{Actions::HOLD};
  PreciseActions next_precise_action{PreciseActions::HOLD};

  bool lost = false;
  bool piece_set = false;

  int hold_piece_index = -1;
  bool hold_used = false;

  Game(int _id);

  // ---------------

  int getId() const;

  int getClearedLines();
  int getLinesClearedLast() const;
  std::array<unsigned int, 4> getClearCounts();

  float getLastLandingHeight() const;
  int getLastErodedPieceCells() const;

  int getScore();

  std::vector<int> getGameState();

  PieceQueue &getPieceQueue();
  const PieceQueue &getPieceQueue() const;

  Board &getBoard();
  const Board &getBoard() const;

  std::array<Position, 4> getLastPiecePlacedPositions() const;

  void increaseScore(int lines);
  void resetScore();

  void setGravity(bool on);
  void tickGravity();

  void hardDrop();

  void holdCurrentPiece();

  int pieceWasSet();

  void gameLost();

  ~Game();
};
