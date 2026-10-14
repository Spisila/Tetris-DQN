#include <rl_env.hpp>
#include <iostream>

RLEnv::RLEnv(int _id) : game_env(_id)
{
}

RLEnv::~RLEnv()
{
}

void RLEnv::takePreciseAction(PreciseActions action)
{
  int precise_action_num = static_cast<int>(action);

  if (precise_action_num <= 39)
  {
    int rotation = precise_action_num % 4;
    int column = precise_action_num / 4;

    game_env.getBoard().moveTetrominoToColumnWithRotation(column, rotation);
    game_env.hardDrop();
  }
  else
  {
    game_env.getBoard().moveTetrominoToColumnWithRotation(3, 1);
    game_env.hardDrop();
  }
}

std::array<std::array<int, 9>, 41> RLEnv::testStep()
{

  int actions_start = static_cast<int>(PreciseActions::ROTATION_0_COLUMN_0);
  int actions_end = static_cast<int>(PreciseActions::HOLD);

  std::array<std::array<int, 9>, 41> state_attempts;

  Game snapshot = game_env;

  for (int i = actions_start; i < actions_end + 1; i++)
  {

    auto precise_action_i = static_cast<PreciseActions>(i);

    takePreciseAction(precise_action_i);

    state_attempts[i] = getGameState();

    // game_env.getBoard().setTetrominoCellsStates(CellState::EMPTY, game_env.getLastPiecePlacedPositions());
    game_env = snapshot;
  }

  return state_attempts;
}

void RLEnv::step()
{
  takePreciseAction(game_env.next_precise_action);
}

void RLEnv::reset()
{
  game_env.lost = false;

  game_env.hold_used = false;
  game_env.hold_piece_index = -1;

  game_env.getBoard().clearBoard();

  game_env.getPieceQueue().generatePieceQueue();
  game_env.getPieceQueue().updatePieceQueue();

  game_env.getBoard().setPiece(static_cast<PieceType>(game_env.getPieceQueue().getPieceQueue().at(0)));
  game_env.getBoard().setTetrominoCellsStates(CellState::ACTIVE, game_env.getBoard().getBoardTetromino().getAllPositions());
}

StepData RLEnv::calculateReward()
{
  if (getGameEnv().lost)
  {
    StepData loss_step = {LOSS_SCORE_WEIGHT, true, true};
    return loss_step;
  }

  if (getGameEnv().piece_set == true)
  {
    getGameEnv().piece_set = false;

    int lines = getGameEnv().getLinesClearedLast();
    int total_height = getGameEnv().getBoard().getAggregateHeight();
    int total_holes = getGameEnv().getBoard().getAmountOfHoles();
    int rugosity = getGameEnv().getBoard().getRugosity();

    // if (lines > 0)
    // {

    //   std::cout << "Lines = " << std::endl;
    //   std::cout << lines << std::endl;

    //   std::cout << "Total height = " << std::endl;
    //   std::cout << total_height << std::endl;

    //   std::cout << "Total holes = " << std::endl;
    //   std::cout << total_holes << std::endl;

    //   std::cout << "Rugosity = " << std::endl;
    //   std::cout << rugosity << std::endl;
    // }

    float reward =
        (lines * LINES_CLEARED_WEIGHT) -
        (total_height * TOTAL_HEIGHT_WEIGHT) -
        (total_holes * HOLES_WEIGHT) -
        (rugosity * RUGOSITY_WEIGHT);

    StepData piece_set_step = {reward, true, false};
    return piece_set_step;
  }

  StepData gravity_step = {-0.005f, false, false};

  return gravity_step;
}

Game &RLEnv::getGameEnv()
{
  return game_env;
}

int RLEnv::getGameEnvID() const
{
  return game_env.getId();
}

std::array<int, 9> RLEnv::getGameState()
{

  std::array<int, 9> state = {};

  std::array<int, 3> board_state = game_env.getBoard().getBoardState();

  for (int i = 0; i < board_state.size(); i++)
  {
    state[i] = board_state[i];
  }

  const Tetromino &current_tetromino = game_env.getBoard().getBoardTetromino();
  const int current_piece_type = current_tetromino.getCurrentPieceType();

  state[3] = formatPiece(current_piece_type);

  const PieceQueue &game_queue = game_env.getPieceQueue();
  const auto &piece_queue = game_queue.getPieceQueue();

  for (int i = 0; i < piece_queue.size(); i++)
  {
    state[i + 4] = formatPiece(piece_queue[i]);
  }

  return state;
}
