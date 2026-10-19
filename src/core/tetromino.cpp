#include <tetromino.hpp>
#include <iostream>

#include <types.hpp>

Tetromino::Tetromino(/* args */)
{
}

Tetromino::~Tetromino()
{
}

int Tetromino::getCurrentPieceTypeFormatted() const
{

  int pieceWithRotation = static_cast<int>(current_piece_type);

  switch (pieceWithRotation)
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

int Tetromino::getCurrentTypeWithRotation() const
{
  if (static_cast<int>(current_piece_type) < 4)
    return 0;
  else if (static_cast<int>(current_piece_type) >= 4 && static_cast<int>(current_piece_type) < 8)
    return 1;
  else if (static_cast<int>(current_piece_type) >= 8 && static_cast<int>(current_piece_type) < 12)
    return 2;
  else if (static_cast<int>(current_piece_type) >= 12 && static_cast<int>(current_piece_type) < 16)
    return 3;
  else if (static_cast<int>(current_piece_type) >= 16 && static_cast<int>(current_piece_type) < 20)
    return 4;
  else if (static_cast<int>(current_piece_type) >= 20 && static_cast<int>(current_piece_type) < 24)
    return 5;
  else if (static_cast<int>(current_piece_type) >= 24 && static_cast<int>(current_piece_type) < 28)
    return 6;
  else
  {
    return 0;
  }
}

int Tetromino::getCurrentPieceType() const
{
  return static_cast<int>(current_piece_type);
}

Position Tetromino::getPivotPosition() const
{
  return pieces_positions[0];
}

const std::array<Position, 4> Tetromino::getAllPositions() const
{
  return pieces_positions;
}

int Tetromino::getCurrentRotation() const
{

  if (getCurrentPieceType() % 4 == 0)
  {
    return 0; // No rotation
  }
  else if (getCurrentPieceType() % 4 == 1)
  {
    return 1; // Rotation 1
  }
  else if (getCurrentPieceType() % 4 == 2)
  {
    return 2; // Rotation 2
  }
  else if (getCurrentPieceType() % 4 == 3)
  {
    return 3; // Rotation 3
  }
  else
  {
    return -1;
  }
}

void Tetromino::setTetrominoType(PieceType new_type)
{
  current_piece_type = new_type;
}

void Tetromino::setPositions(std::array<Position, 4> new_pos)
{

  for (int i = 0; i < new_pos.size(); i++)
  {
    pieces_positions[i] = new_pos[i];
  }
}

// index + (current +/- 1) mod(4)
PieceType Tetromino::changeTetrominoeRotation(Rotation rot)
{

  int current_index = static_cast<int>(current_piece_type);
  int general_index = 0;

  if (current_index < 4)
    general_index = 0;
  else if (current_index >= 4 && current_index < 8)
    general_index = 4;
  else if (current_index >= 8 && current_index < 12)
    general_index = 8;
  else if (current_index >= 12 && current_index < 16)
    general_index = 12;
  else if (current_index >= 16 && current_index < 20)
    general_index = 16;
  else if (current_index >= 20 && current_index < 24)
    general_index = 20;
  else if (current_index >= 24 && current_index < 28)
    general_index = 24;
  else
  {
    general_index = 0;
  }

  int step;

  switch (rot)
  {
  case Rotation::CLOCKWISE:
    step = 1;
    break;
  case Rotation::COUNTER_CLOCKWISE:
    step = -1;
    break;
  case Rotation::ONE_EIGHTY:
    step = 2;
    break;
  default:
    break;
  }

  int new_rotated_piece = general_index + ((static_cast<int>(current_piece_type) + step) % 4);

  if (new_rotated_piece < 0)
    new_rotated_piece += 4;

  return static_cast<PieceType>(new_rotated_piece);
}

std::array<Position, 4> Tetromino::projectMovement(MovementDirection dir)
{

  std::array<Position, 4> projected_position{
      Position(0, 0),
      Position(0, 0),
      Position(0, 0),
      Position(0, 0)};

  // 0 is the pivot of the projected tetromino
  projected_position[0] = pieces_positions[0];
  switch (dir)
  {
  case MovementDirection::UP:
    projected_position[0].y--;
    break;
  case MovementDirection::DOWN:
    projected_position[0].y++;
    break;
  case MovementDirection::LEFT:
    projected_position[0].x--;
    break;
  case MovementDirection::RIGHT:
    projected_position[0].x++;
    break;
  case MovementDirection::TOP_RIGHT:
    projected_position[0].x++;
    projected_position[0].y--;
    break;
  case MovementDirection::TOP_LEFT:
    projected_position[0].x--;
    projected_position[0].y--;
    break;
  case MovementDirection::BOTTOM_RIGHT:
    projected_position[0].x++;
    projected_position[0].y++;
    break;
  case MovementDirection::BOTTOM_LEFT:
    projected_position[0].x--;
    projected_position[0].y++;
    break;
  default:
    break;
  }

  std::array<Position, 3> pos = rotationToPositionOffsets.at(current_piece_type);

  for (int j = 0; j < pos.size(); j++)
  {
    projected_position[j + 1].x = projected_position[0].x + pos[j].x;
    projected_position[j + 1].y = projected_position[0].y + pos[j].y;
  }

  return projected_position;
}

std::array<Position, 4> Tetromino::projectRotation(Rotation rot)
{

  std::array<Position, 4> projected_rotation{
      Position(0, 0),
      Position(0, 0),
      Position(0, 0),
      Position(0, 0)};

  PieceType new_rotation = changeTetrominoeRotation(rot);

  projected_rotation.at(0) = pieces_positions.at(0);

  std::array<Position, 3> pos = rotationToPositionOffsets.at(new_rotation);

  for (int i = 0; i < pos.size(); i++)
  {
    projected_rotation[i + 1].x = projected_rotation[0].x + pos[i].x;
    projected_rotation[i + 1].y = projected_rotation[0].y + pos[i].y;
  }

  return projected_rotation;
}
