#pragma once

#include <array>
#include <tetromino.hpp>
#include <algorithm>

#include <random>

#include <types.hpp>

constexpr size_t queue_size = 5;

class PieceQueue
{
private:
  int queue_index{0};

  std::array<PieceType, 14> piece_queue;
  std::array<PieceType, 5> known_piece_queue;

  std::mt19937 gen;

  int seed;

public:
  PieceQueue(int seed) : gen(1) {};
  ~PieceQueue();

  size_t getQueueSize() const;
  const std::array<int, queue_size> getPieceQueue() const;
  int getQueueIndex() const;

  void generatePieceQueue();
  void updatePieceQueue();
};
