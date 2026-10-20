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

  std::vector<std::array<py::array_t<float>, ACTION_SPACE_SIZE>> look_ahead_cache;

  bool running{false};

  static std::size_t getThreadCount();
  static std::vector<RLEnv> createGames(size_t count);

public:
  std::vector<py::array_t<float>> states_cache;

  explicit MultiGame(size_t parallel_game_count);
  ~MultiGame();

  void threadLoop(size_t thread_segment_index);

  std::vector<StepData> stepAll(std::vector<PreciseActions> _actions);
  void stepThis(int index, Actions _action);

  const std::vector<std::array<py::array_t<float>, ACTION_SPACE_SIZE>> &lookAheadAll();

  void resetAll();
  void resetThis(int index);

  void checkPiecePlaced();
  void checkGameLost();

  int getSumScore();
  int getLinesCleared();
  std::array<unsigned int, 4> getAllClearCounts();

  std::vector<py::array_t<float>> getStates();

  void initGraphics();
  void render(std::string &generation_counter);
  void closeGraphics();
};
