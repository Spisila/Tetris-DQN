
#include <iostream>
#include <pybind11/pybind11.h>

namespace py = pybind11;

#include <rl/multi_game.hpp>

MultiGame::MultiGame(size_t parallel_game_count)
    : barrier(std::thread::hardware_concurrency() + 1),
      games([](size_t count)
            { 
          std::vector<RLEnv> v; 
          v.reserve(count); 
          return v; }(parallel_game_count)),
      renderer((games.emplace_back(0), games[0].getGameEnv()))
{

  states_cache.push_back(games[0].getGameState());

  GAME_AMOUNT = parallel_game_count;

  running = true;

  unsigned int num_cores = std::thread::hardware_concurrency();

  for (int i = 1; i < GAME_AMOUNT; i++)
  {
    games.emplace_back(i);
    states_cache.push_back(games[i].getGameState());
  }

  int segment_size = GAME_AMOUNT / num_cores;

  std::span<RLEnv> game_view{games};

  for (size_t i = 0; i < num_cores; i++)
  {

    if (i == num_cores - 1)
    {
      segments.push_back(game_view.subspan(i * segment_size));
    }
    else
    {

      segments.push_back(game_view.subspan(i * segment_size, segment_size));
    }

    game_workers.push_back(std::thread(&MultiGame::threadLoop, this, i));
  }
}

MultiGame::~MultiGame()
{

  barrier.arrive_and_wait();
  running = false;

  for (auto &worker : game_workers)
  {
    if (worker.joinable())
    {
      worker.join();
    }
  }
}

void MultiGame::threadLoop(size_t thread_segment_index)
{

  while (running)
  {
    barrier.arrive_and_wait();

    if (!running)
    {
      break;
    }

    // TODO: Move this outside the loop maybe?
    auto thread_segment = segments[thread_segment_index];

    for (auto &game : thread_segment)
    {
      game.step();
    }

    barrier.arrive_and_wait();
  }
}

std::vector<StepData> MultiGame::stepAll(std::vector<PreciseActions> _actions)
{

  py::gil_scoped_release release;

  for (int i = 0; i < games.size(); i++)
  {
    games[i].getGameEnv().next_precise_action = _actions[i];
  }

  barrier.arrive_and_wait();

  barrier.arrive_and_wait();

  std::vector<StepData> rewards;

  for (int i = 0; i < games.size(); i++)
  {
    rewards.push_back(games[i].calculateReward());
    states_cache[games[i].getGameEnvID()] = games[i].getGameState();
  }

  return rewards;
}

void MultiGame::stepThis(int index, Actions _action)
{
  games[index].step();
}

// 9  = state values
// 41 = Possible actions
// 8 = Amount of games (not dynamic)
std::array<std::array<std::array<int, 12>,41>, 8> 
MultiGame::lookAheadAll()
{

  std::array<std::array<std::array<int, 12>,41>, 8> look_ahead = {};

  for (int i = 0; i < 8; i++) {
    look_ahead[i] = games[i].testStep();
  }

  return look_ahead;

}


void MultiGame::resetAll()
{

  for (int i = 0; i < games.size(); i++)
  {
    games[i].reset();
  }
}

void MultiGame::resetThis(int index)
{

  games[index].reset();
}

// int MultiGame::getSumScore()
// {
//   int sum = 0;
//   for (Game &game : games)
//   {
//     sum += game.get_score();
//   }
//   return sum;
// }

int MultiGame::getLinesCleared()
{
  int sum = 0;
  for (RLEnv &game : games)
  {
    sum += game.getGameEnv().getClearedLines();
  }
  return sum;
}

std::vector<std::array<int, 12>> MultiGame::getStates()
{
  return states_cache;
}

void MultiGame::initGraphics()
{
  renderer.initWindow();
}

void MultiGame::render(std::string &generation_counter)
{
  renderer.drawLoop();
}

void MultiGame::closeGraphics()
{
  renderer.closeWindow();
}
