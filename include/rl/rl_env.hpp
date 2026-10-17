#pragma once

#include <types.hpp>
#include <game.hpp>

// TODO: TEST with lower loss weight
constexpr float LOSS_SCORE_WEIGHT = -100.0f;

class RLEnv
{
private:
  Game game_env;

public:
  RLEnv(int _id);
  ~RLEnv();

  Game &getGameEnv();
  int getGameEnvID() const;
  std::array<float, STATE_SIZE> getGameState();

  void takePreciseAction(PreciseActions action);

  std::array<std::array<float, STATE_SIZE>, ACTION_SPACE_SIZE> testStep();

  void step();
  void reset();

  StepData calculateReward();
};
