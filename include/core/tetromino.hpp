#pragma once

#include <unordered_map>
#include <shared.hpp>
#include <array>

#include <shared.hpp>
#include <types.hpp>

inline std::unordered_map<PieceType, std::array<Position, 3>> rotationToPositionOffsets = {

    {PieceType::I_PIECE_0, std::array<Position, 3>{Position(-1, 0), Position(+1, 0), Position(+2, 0)}},
    {PieceType::I_PIECE_R, std::array<Position, 3>{Position(0, -1), Position(0, +1), Position(0, +2)}},
    {PieceType::I_PIECE_2, std::array<Position, 3>{Position(-1, 0), Position(-2, 0), Position(+1, 0)}},
    {PieceType::I_PIECE_L, std::array<Position, 3>{Position(0, +1), Position(0, -1), Position(0, -2)}},

    {PieceType::J_PIECE_0, std::array<Position, 3>{Position(-1, 0), Position(-1, -1), Position(+1, 0)}},
    {PieceType::J_PIECE_R, std::array<Position, 3>{Position(0, -1), Position(+1, -1), Position(0, +1)}},
    {PieceType::J_PIECE_2, std::array<Position, 3>{Position(-1, 0), Position(+1, 0), Position(+1, +1)}},
    {PieceType::J_PIECE_L, std::array<Position, 3>{Position(0, -1), Position(0, +1), Position(-1, +1)}},

    {PieceType::L_PIECE_0, std::array<Position, 3>{Position(-1, 0), Position(+1, 0), Position(+1, -1)}},
    {PieceType::L_PIECE_R, std::array<Position, 3>{Position(0, -1), Position(0, +1), Position(+1, +1)}},
    {PieceType::L_PIECE_2, std::array<Position, 3>{Position(-1, +1), Position(-1, 0), Position(+1, 0)}},
    {PieceType::L_PIECE_L, std::array<Position, 3>{Position(-1, -1), Position(0, -1), Position(0, +1)}},

    {PieceType::O_PIECE_0, std::array<Position, 3>{Position(+1, 0), Position(+1, -1), Position(0, -1)}},
    {PieceType::O_PIECE_R, std::array<Position, 3>{Position(+1, 0), Position(+1, +1), Position(0, +1)}},
    {PieceType::O_PIECE_2, std::array<Position, 3>{Position(-1, 0), Position(-1, +1), Position(0, +1)}},
    {PieceType::O_PIECE_L, std::array<Position, 3>{Position(-1, 0), Position(-1, -1), Position(0, -1)}},

    {PieceType::S_PIECE_0, std::array<Position, 3>{Position(-1, 0), Position(0, -1), Position(+1, -1)}},
    {PieceType::S_PIECE_R, std::array<Position, 3>{Position(0, -1), Position(+1, 0), Position(+1, +1)}},
    {PieceType::S_PIECE_2, std::array<Position, 3>{Position(-1, +1), Position(0, +1), Position(+1, 0)}},
    {PieceType::S_PIECE_L, std::array<Position, 3>{Position(-1, -1), Position(-1, 0), Position(0, +1)}},

    {PieceType::T_PIECE_0, std::array<Position, 3>{Position(-1, 0), Position(0, -1), Position(+1, 0)}},
    {PieceType::T_PIECE_R, std::array<Position, 3>{Position(0, -1), Position(0, +1), Position(+1, 0)}},
    {PieceType::T_PIECE_2, std::array<Position, 3>{Position(-1, 0), Position(0, +1), Position(+1, 0)}},
    {PieceType::T_PIECE_L, std::array<Position, 3>{Position(-1, 0), Position(0, -1), Position(0, +1)}},

    {PieceType::Z_PIECE_0, std::array<Position, 3>{Position(-1, -1), Position(0, -1), Position(+1, 0)}},
    {PieceType::Z_PIECE_R, std::array<Position, 3>{Position(+1, 0), Position(0, +1), Position(+1, -1)}},
    {PieceType::Z_PIECE_2, std::array<Position, 3>{Position(-1, 0), Position(0, +1), Position(+1, +1)}},
    {PieceType::Z_PIECE_L, std::array<Position, 3>{Position(-1, 0), Position(-1, +1), Position(0, -1)}}

};

class Tetromino
{
private:
  std::array<Position, 4> pieces_positions{Position(5, 1), Position(0, 0), Position(0, 0), Position(0, 0)};
  PieceType current_piece_type = PieceType::I_PIECE_0;

public:
  Tetromino(/* args */);
  ~Tetromino();

  int getCurrentPieceTypeFormatted() const;
  int getCurrentPieceType() const;
  int getCurrentTypeWithRotation() const;

  Position getPivotPosition() const;
  const std::array<Position, 4> getAllPositions() const;
  int getCurrentRotation() const;

  void setTetrominoType(PieceType new_type);

  void setPositions(std::array<Position, 4> new_pos);
  PieceType changeTetrominoeRotation(Rotation rot);

  std::array<Position, 4> projectMovement(MovementDirection dir);
  std::array<Position, 4> projectRotation(Rotation rot);
};

