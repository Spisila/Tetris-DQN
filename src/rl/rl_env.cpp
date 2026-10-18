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

  int rotation = precise_action_num % 4;
  int column = precise_action_num / 4;

  game_env.getBoard().moveTetrominoToColumnWithRotation(column, rotation);
  game_env.hardDrop();
}

std::array<py::array_t<float>, ACTION_SPACE_SIZE> RLEnv::testStep()
{

  int actions_start = static_cast<int>(PreciseActions::ROTATION_0_COLUMN_0);
  int actions_end = static_cast<int>(PreciseActions::ROTATION_3_COLUMN_9);

  std::array<py::array_t<float>, ACTION_SPACE_SIZE> state_attempts;

  Game snapshot = game_env;

  for (int i = actions_start; i < actions_end + 1; i++)
  {

    auto precise_action_i = static_cast<PreciseActions>(i);

    takePreciseAction(precise_action_i);

    state_attempts[i] = getGameState();

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
    StepData loss_step = {-20.f, true, true};
    return loss_step;
  }

  if (getGameEnv().piece_set == true)
  {
    getGameEnv().piece_set = false;

    int lines = getGameEnv().getLinesClearedLast();
    // float landing_height = getGameEnv().getLastLandingHeight();
    // int eroded_cells = getGameEnv().getLastErodedPieceCells();
    // int row_transitions = getGameEnv().getBoard().getRowTransitions();
    // int column_transitions = getGameEnv().getBoard().getColumnTransitions();
    // int holes = getGameEnv().getBoard().getAmountOfHoles();
    // int wells = getGameEnv().getBoard().getWellSums();

    // float reward =
    //     (-1 * landing_height) +
    //     (2 * eroded_cells) +
    //     (-1 * row_transitions) +
    //     (-1 * column_transitions) +
    //     (-4 * holes) +
    //     (-1 * wells);

    float reward = 1.0f + (lines * lines) * 10.0f;

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

float RLEnv::sigmoidScale(float x, float k) const
{
  return (2.0f / (1.0f + exp(-x / k))) - 1;
}

py::array_t<float> RLEnv::getGameState()
{

  /*
  STATE :
  0 = eroded_cells
  1 = landing_height
  2 = row_transitions
  3 = column_transitions
  4 = holes
  5 = wells
  6: = current piece and piece queue
  */

  std::array<float, STATE_SIZE> state = {};

  int state_index = 0;

  float eroded_cells = static_cast<float>(game_env.getLastErodedPieceCells());
  float landing_height = game_env.getLastLandingHeight();

  state[state_index++] = sigmoidScale(eroded_cells, 4);
  state[state_index++] = sigmoidScale(landing_height, 10);

  std::array<int, 4> board_state = game_env.getBoard().getBoardState();

  state[state_index++] = sigmoidScale(static_cast<float>(board_state[0]), 52);
  state[state_index++] = sigmoidScale(static_cast<float>(board_state[1]), 30);
  state[state_index++] = sigmoidScale(static_cast<float>(board_state[2]), 12);
  state[state_index++] = sigmoidScale(static_cast<float>(board_state[3]), 19);

  auto column_heights = game_env.getBoard().getBoardHeight();

  for (int height : column_heights) {
    state[state_index++] = static_cast<float>(height) / BOARD_SIZE_Y;
  } 
  
  auto height_differences = game_env.getBoard().getColumnHeightDifferences();
  
  for (int diff : height_differences) {
    state[state_index++] = sigmoidScale(static_cast<float>(diff), 3.5f);
  } 

  const Tetromino &current_tetromino = game_env.getBoard().getBoardTetromino();
  const int current_piece_type = current_tetromino.getCurrentPieceType();

  int piece_id = formatPiece(current_piece_type);

  const auto &current_piece_hot_one = ONE_HOT_PIECES_TABLE[piece_id];

  for (float f : current_piece_hot_one)
  {
    state[state_index++] = f;
  }

  const PieceQueue &game_queue = game_env.getPieceQueue();
  const auto &piece_queue = game_queue.getPieceQueue();

  for (int piece : piece_queue)
  {
    int queue_piece_id = formatPiece(piece);
    const auto &queue_piece_hot_one = ONE_HOT_PIECES_TABLE[queue_piece_id];
    for (float q_i : queue_piece_hot_one)
    {
      state[state_index++] = q_i;
    }
  }

  return py::array_t<float>(
      STATE_SIZE,
      state.data());

  // return state;
}
