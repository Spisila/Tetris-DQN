#pragma once

#include <types.hpp>
#include <game.hpp>

// TODO: Pass these to python for testing
constexpr float LOSS_SCORE_WEIGHT = -100.0f;
constexpr float LINES_CLEARED_WEIGHT = 200.0f;
constexpr float TOTAL_HEIGHT_WEIGHT = 0.5f;
constexpr float HOLES_WEIGHT = 4.0f;
constexpr float RUGOSITY_WEIGHT = 0.2f;

class RLEnv
{
private:
  Game game_env;

public:
  RLEnv(int _id);
  ~RLEnv();

  Game &getGameEnv();
  int getGameEnvID() const;
  std::array<int, 12> getGameState();

  void takePreciseAction(PreciseActions action);

  std::array<std::array<int, 12>, 41> testStep();

  void step();
  void reset();

  StepData calculateReward();
};
