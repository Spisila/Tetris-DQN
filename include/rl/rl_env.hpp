#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <types.hpp>
#include <game.hpp>
#include <cmath>

namespace py = pybind11;

constexpr float LOSS_SCORE_WEIGHT = -10.0f;

constexpr std::array<std::array<float, 7>, 7> ONE_HOT_PIECES_TABLE = {{
  {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, // ID 0 (ex: I)
  {0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, // ID 1 (ex: J)
  {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f}, // ID 2 (ex: L)
  {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f}, // ID 3 (ex: O)
  {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}, // ID 4 (ex: S)
  {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f}, // ID 5 (ex: T)
  {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}  // ID 6 (ex: Z)
}};

class RLEnv
{
  private:
  Game game_env;

public:
  RLEnv(int _id);
  ~RLEnv();

  Game &getGameEnv();
  int getGameEnvID() const;
  py::array_t<float> getGameState();

  void takePreciseAction(PreciseActions action);

  std::array<py::array_t<float>, ACTION_SPACE_SIZE> testStep();

  void step();
  void reset();

  StepData calculateReward();

  float sigmoidScale(float x, float k) const;
};
