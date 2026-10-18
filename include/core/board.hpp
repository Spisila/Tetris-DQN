#pragma once

#include <array>
#include <tetromino.hpp>

#include <shared.hpp>
#include <types.hpp>

constexpr int BOARD_SIZE_X = 10;
constexpr int BOARD_SIZE_Y = 24;

class Board
{
private:
  std::array<std::array<CellState, BOARD_SIZE_Y>, BOARD_SIZE_X> board;
  Tetromino tetromino{};

  // TODO: make spawn positions a single position for the pivot
  // TODO: make spawn position scale with board size
  std::array<Position, 4> spawn_positions = {
      Position(5, 2),
      Position(0, 0),
      Position(0, 0),
      Position(0, 0)};

public:
  Board(/* args */);
  ~Board();

  // Getters

  std::array<std::array<CellState, BOARD_SIZE_Y>, BOARD_SIZE_X> getBoardCells() const;

  std::array<int, BOARD_SIZE_X> getBoardHeight() const;
  std::array<int, BOARD_SIZE_X> getColumnTopHeights() const;
  std::array<int, BOARD_SIZE_X - 1> getColumnHeightDifferences() const;

  int getAmountOfHoles() const;
  int getRugosity() const;
  int getRowTransitions() const;
  int getColumnTransitions() const;
  int getWellSums() const;

  float calculateLandingHeight(const std::array<Position, 4> &piece_positions) const;
  int calculateErodedPieceCells(const std::array<Position, 4> &piece_positions) const;
  
  
  const std::array<int, 4> getBoardState() const;
  
  int getAggregateHeight() const;
  
  Tetromino &getBoardTetromino();
  const Tetromino &getBoardTetromino() const;

  // Setters

  void setTetrominoCellsStates(CellState state, std::array<Position, 4> positions);

  // Methods

  bool moveTetromino(MovementDirection dir);
  void moveTetrominoToColumn(int column);
  bool rotateTetromino(Rotation rot);
  void rotateTetrominoToRotation(int rotation);

  void moveTetrominoToColumnWithRotation(int column, int rotation);

  bool setPiece(PieceType new_piece);

  bool checkOutOfLateralBounds(std::array<Position, 4> projected_positions);
  bool checkCollisions(std::array<Position, 4> projected_positions);
  bool checkTouchedFloor(std::array<Position, 4> projected_positions);

  bool checkShouldSetPiece(std::array<Position, 4> projected_position);

  void tickGravity();
  int clearLines();

  void clearBoard();
};


