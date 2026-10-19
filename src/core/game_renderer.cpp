#include <game_renderer.hpp>

GameRenderer::GameRenderer(Game &_game) : game(_game)
{
}
Color GameRenderer::pieceToColor(CellState piece)
{
  switch (piece)
  {
  case CellState::FILLED_I:
    return SKYBLUE;
  case CellState::FILLED_O:
    return YELLOW;
  case CellState::FILLED_T:
    return PURPLE;
  case CellState::FILLED_S:
    return GREEN;
  case CellState::FILLED_Z:
    return RED;
  case CellState::FILLED_J:
    return DARKBLUE;
  case CellState::FILLED_L:
    return ORANGE;
  default:
    return BOARD_COLOR;
  }
}

void GameRenderer::drawBoard(const Game &game)
{

  const auto &board = game.getBoard();
  auto cells = board.getBoardCells();

  for (int x = 0; x < cells.size(); x++)
  {
    for (int y = 0; y < cells[x].size(); y++)
    {
      Color cell_color;

      if (y < 4)
      {
        cell_color = SPAWN_COLOR;
      }
      else
      {
        cell_color = BOARD_COLOR;
      }

      if (board.isCellFilled(x, y))
      {
        cell_color = pieceToColor(cells[x][y]);
      }

      if (cells[x][y] == CellState::ACTIVE)
      {
        int active_piece = static_cast<int>(board.getBoardTetromino().getCurrentPieceType());

        if (active_piece < 4)
          cell_color = SKYBLUE;
        else if (active_piece >= 4 && active_piece < 8)
          cell_color = DARKBLUE;
        else if (active_piece >= 8 && active_piece < 12)
          cell_color = ORANGE;
        else if (active_piece >= 12 && active_piece < 16)
          cell_color = YELLOW;
        else if (active_piece >= 16 && active_piece < 20)
          cell_color = GREEN;
        else if (active_piece >= 20 && active_piece < 24)
          cell_color = PURPLE;
        else if (active_piece >= 24 && active_piece < 28)
          cell_color = RED;
      }

      DrawRectangle(OFFSET_X + x * CELL_SIZE, OFFSET_Y + y * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, cell_color);
    }
  }
}

void GameRenderer::initWindow()
{
  SetTargetFPS(60);

  SetTraceLogLevel(LOG_WARNING);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GEORGE TETRIS");
}

void GameRenderer::drawLoop()
{

  BeginDrawing();

  ClearBackground(BACKGROUND_COLOR);

  drawBoard(game);

  drawScoreText();
  drawClearCounts();

  EndDrawing();
}

void GameRenderer::drawClearCounts()
{
  auto clears = game.getClearCounts();

  DrawText(TextFormat("SINGLES = %d", clears[0]), SCREEN_WIDTH / 2 + 65, SCREEN_HEIGHT / 2, 30, WHITE);
  DrawText(TextFormat("DOUBLES = %d", clears[1]), SCREEN_WIDTH / 2 + 65, SCREEN_HEIGHT / 2 + 50, 30, WHITE);
  DrawText(TextFormat("TRIPLES = %d", clears[2]), SCREEN_WIDTH / 2 + 65, SCREEN_HEIGHT / 2 + 100, 30, WHITE);
  DrawText(TextFormat("TETRIS  = %d", clears[3]), SCREEN_WIDTH / 2 + 65, SCREEN_HEIGHT / 2 + 150, 30, WHITE);
}

void GameRenderer::drawScoreText()
{
  DrawText(TextFormat("SCORE = %d", game.getScore()), SCREEN_WIDTH / 2 - 260, SCREEN_HEIGHT / 2 - 375, 30, WHITE);
}

void GameRenderer::closeWindow()
{
  CloseWindow();
}

void GameRenderer::drawHoldedPiece()
{
  DrawText(TextFormat("HOLD = %c", pieceTypeToChar(static_cast<PieceType>(game.hold_piece_index))), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT / 2 - 400, 30, RED);
}

void GameRenderer::drawPieceQueue()
{
  DrawText(TextFormat("QUEUE = %c %c %c %c %c",
                      pieceTypeToChar(static_cast<PieceType>(game.getPieceQueue().getPieceQueue().at(0))),
                      pieceTypeToChar(static_cast<PieceType>(game.getPieceQueue().getPieceQueue().at(1))),
                      pieceTypeToChar(static_cast<PieceType>(game.getPieceQueue().getPieceQueue().at(2))),
                      pieceTypeToChar(static_cast<PieceType>(game.getPieceQueue().getPieceQueue().at(3))),
                      pieceTypeToChar(static_cast<PieceType>(game.getPieceQueue().getPieceQueue().at(4)))),
           SCREEN_WIDTH / 2 + 400, SCREEN_HEIGHT / 2 - 320, 30, RED);
}

char GameRenderer::pieceTypeToChar(PieceType piece)
{
  switch (piece)
  {
  case PieceType::I_PIECE_0:
    return 'I';
    break;

  case PieceType::J_PIECE_0:

    return 'J';
    break;

  case PieceType::L_PIECE_0:
    return 'L';
    break;

  case PieceType::O_PIECE_0:

    return 'O';
    break;

  case PieceType::S_PIECE_0:

    return 'S';
    break;

  case PieceType::T_PIECE_0:

    return 'T';
    break;

  case PieceType::Z_PIECE_0:

    return 'Z';
    break;

  default:
    return '?';
    break;
  }
}