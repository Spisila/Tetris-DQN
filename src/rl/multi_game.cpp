
#include <iostream>
#include <pybind11/pybind11.h>

namespace py = pybind11;

#include <rl/multi_game.hpp>

MultiGame::MultiGame(size_t parallel_game_count)
    : barrier(getThreadCount()),
      games(createGames(parallel_game_count)),
      renderer(games[0].getGameEnv())
{

  GAME_AMOUNT = parallel_game_count;
  look_ahead_cache.resize(GAME_AMOUNT);

  running = true;

  for (int i = 0; i < GAME_AMOUNT; i++)
  {
    states_cache.push_back(games[i].getGameState());
  }

  unsigned int num_cores = std::thread::hardware_concurrency();
  size_t active_threads = std::min(static_cast<size_t>(num_cores), GAME_AMOUNT);
  size_t segment_size = GAME_AMOUNT / num_cores;

  std::span<RLEnv> game_view{games};

  for (size_t i = 0; i < active_threads; i++)
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

std::size_t MultiGame::getThreadCount()
{
  auto cores = std::thread::hardware_concurrency();
  return (cores == 0 ? 1 : cores) + 1;
}

std::vector<RLEnv> MultiGame::createGames(size_t count)
{
  if (count == 0)
  {
    throw std::invalid_argument("parallel_game_count deve ser maior que 0.");
  }

  std::vector<RLEnv> envs;
  envs.reserve(count);

  for (size_t i = 0; i < count; ++i)
  {
    envs.emplace_back(i);
  }

  return envs;
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

const std::vector<std::array<std::array<float, STATE_SIZE>, ACTION_SPACE_SIZE>>& MultiGame::lookAheadAll()
{

  for (size_t i = 0; i < games.size(); i++)
  {
    look_ahead_cache[i] = games[i].testStep();
  }

  return look_ahead_cache;
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

std::vector<std::array<float, 12>> MultiGame::getStates()
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
