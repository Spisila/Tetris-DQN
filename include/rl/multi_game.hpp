#pragma once

#include <array>
#include <vector>

#include <span>
#include <thread>
#include <future>

#include <barrier>

#include <game.hpp>
#include <game_renderer.hpp>
#include <rl_env.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

class MultiGame
{
private:

  size_t GAME_AMOUNT = 8;

  std::barrier<> barrier;
  std::vector<RLEnv> games;
  
  std::vector<std::span<RLEnv>> segments;
  std::vector<std::thread> game_workers;
  
  GameRenderer renderer;

  std::vector<std::array<std::array<int, 12>, 41>> look_ahead_cache;

  bool running{false};

  static std::size_t getThreadCount();
  static std::vector<RLEnv> createGames(size_t count);


public:
  std::vector<std::array<int, 12>> states_cache;

  explicit MultiGame(size_t parallel_game_count);
  ~MultiGame();

  void threadLoop(size_t thread_segment_index);

  std::vector<StepData> stepAll(std::vector<PreciseActions> _actions);
  void stepThis(int index, Actions _action);

  // TODO: Make it work with variable game amount
  const std::vector<std::array<std::array<int, 12>, 41>>& lookAheadAll();

  void resetAll();
  void resetThis(int index);

  void checkPiecePlaced();
  void checkGameLost();

  int getSumScore();
  int getLinesCleared();

  std::vector<std::array<int, 12>> getStates();

  void initGraphics();
  void render(std::string &generation_counter);
  void closeGraphics();
  

};


