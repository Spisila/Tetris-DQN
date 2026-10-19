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

CellType GameRenderer::pieceToCellType(CellState piece)
{
  switch (piece)
  {
  case CellState::FILLED_I:
    return CellType::FILLED_I;
  case CellState::FILLED_O:
    return CellType::FILLED_O;
  case CellState::FILLED_T:
    return CellType::FILLED_T;
  case CellState::FILLED_S:
    return CellType::FILLED_S;
  case CellState::FILLED_Z:
    return CellType::FILLED_Z;
  case CellState::FILLED_J:
    return CellType::FILLED_J;
  case CellState::FILLED_L:
    return CellType::FILLED_L;
  default:
    return CellType::EMPTY;
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
      CellType cell_type;

      if (y < 4)
      {
        cell_type = CellType::SPAWN;
      }
      else
      {
        cell_type = CellType::EMPTY;
      }

      if (board.isCellFilled(x, y))
      {
        cell_color = pieceToColor(cells[x][y]);
        cell_type = pieceToCellType(cells[x][y]);
      }

      if (cells[x][y] == CellState::ACTIVE)
      {
        const int active_piece = static_cast<int>(board.getBoardTetromino().getCurrentPieceType());

        const int piece_index = active_piece / 4;

        switch (piece_index)
        {
        case 0: // I
          cell_type = CellType::FILLED_I;
          break;

        case 1: // J
          cell_type = CellType::FILLED_J;
          break;

        case 2: // L
          cell_type = CellType::FILLED_L;
          break;

        case 3: // O
          cell_type = CellType::FILLED_O;
          break;

        case 4: // S
          cell_type = CellType::FILLED_S;
          break;

        case 5: // T
          cell_type = CellType::FILLED_T;
          break;

        case 6: // Z
          cell_type = CellType::FILLED_Z;
          break;
        }
      }

      switch (cell_type)
      {
      case CellType::EMPTY:
        drawCell(x, y, BOARD_COLOR, BOARD_SHADOW, BOARD_HALFTONE, BOARD_HIGHLIGHT);
        break;

      case CellType::SPAWN:
        drawCell(x, y, SPAWN_COLOR, SPAWN_SHADOW, SPAWN_HALFTONE, SPAWN_HIGHLIGHT);
        break;

      case CellType::FILLED_I:
        drawCell(x, y, I_HALFTONE, I_HIGHLIGHT, SKYBLUE, I_SHADOW);
        break;

      case CellType::FILLED_J:
        drawCell(x, y, J_HALFTONE, J_HIGHLIGHT, DARKBLUE, J_SHADOW);
        break;

      case CellType::FILLED_L:
        drawCell(x, y, L_HALFTONE, L_HIGHLIGHT, ORANGE, L_SHADOW);
        break;

      case CellType::FILLED_O:
        drawCell(x, y, O_HALFTONE, O_HIGHLIGHT, YELLOW, O_SHADOW);
        break;

      case CellType::FILLED_S:
        drawCell(x, y, S_HALFTONE, S_HIGHLIGHT, GREEN, S_SHADOW);
        break;

      case CellType::FILLED_T:
        drawCell(x, y, T_HALFTONE, T_HIGHLIGHT, PURPLE, T_SHADOW);
        break;

      case CellType::FILLED_Z:
        drawCell(x, y, Z_HALFTONE, Z_HIGHLIGHT, RED, Z_SHADOW);
        break;

      default:
        break;
      }
    }
  }
}

void GameRenderer::drawCell(int x, int y, Color base_color, Color shadow, Color halftone, Color highlight)
{
  auto top_left_x = OFFSET_X + x * CELL_SIZE;
  auto top_left_y = OFFSET_Y + y * CELL_SIZE;
  auto bottom_right_x = top_left_x + CELL_SIZE - 1;
  auto bottom_right_y = top_left_y + CELL_SIZE - 1;

  DrawRectangle(top_left_x, top_left_y, CELL_SIZE - 1, CELL_SIZE - 1, base_color);
  DrawRectangle(top_left_x + 5, top_left_y + 5, CELL_SIZE - 10, CELL_SIZE - 10, halftone);

  Vector2 out_cell_top_left{static_cast<float>(top_left_x), static_cast<float>(top_left_y)};
  Vector2 out_cell_bottom_right{static_cast<float>(bottom_right_x), static_cast<float>(bottom_right_y)};

  Vector2 in_cell_top_left{static_cast<float>(top_left_x + 5), static_cast<float>(top_left_y + 5)};
  Vector2 in_cell_bottom_right{static_cast<float>(bottom_right_x - 5), static_cast<float>(bottom_right_y - 5)};

  drawTrapezoidTop(out_cell_top_left, out_cell_bottom_right, in_cell_top_left, in_cell_bottom_right, shadow);
  drawTrapezoidBottom(out_cell_top_left, out_cell_bottom_right, in_cell_top_left, in_cell_bottom_right, highlight);
  // drawTrapezoidRight(out_cell_top_left, out_cell_bottom_right, in_cell_top_left, in_cell_bottom_right, BOARD_SHADOW);
  // drawTrapezoidLeft(out_cell_top_left, out_cell_bottom_right, in_cell_top_left, in_cell_bottom_right, BOARD_SHADOW);
}

void GameRenderer::drawTrapezoidTop(Vector2 out_top_left, Vector2 out_bottom_right, Vector2 in_top_left, Vector2 in_bottom_right, Color color)
{
  // Define os 4 pontos do trapézio
  Vector2 pA = {out_top_left.x, out_top_left.y};     // Superior Esquerdo Externo
  Vector2 pB = {in_top_left.x, in_top_left.y};       // Superior Esquerdo Interno
  Vector2 pC = {in_bottom_right.x, in_top_left.y};   // Superior Direito Interno
  Vector2 pD = {out_bottom_right.x, out_top_left.y}; // Superior Direito Externo

  // Primeiro Triângulo (Esquerdo/Central): ordem anti-horária (A -> B -> C)
  DrawTriangle(pA, pB, pC, color);

  // Segundo Triângulo (Direito/Central): ordem anti-horária (A -> C -> D)
  DrawTriangle(pA, pC, pD, color);
}

void GameRenderer::drawTrapezoidBottom(Vector2 out_top_left, Vector2 out_bottom_right, Vector2 in_top_left, Vector2 in_bottom_right, Color color)
{
  // Define os 4 pontos da face inferior
  Vector2 pA = {out_top_left.x, out_bottom_right.y};     // Inferior Esquerdo Externo
  Vector2 pB = {in_top_left.x, in_bottom_right.y};       // Inferior Esquerdo Interno
  Vector2 pC = {in_bottom_right.x, in_bottom_right.y};   // Inferior Direito Interno
  Vector2 pD = {out_bottom_right.x, out_bottom_right.y}; // Inferior Direito Externo

  // Ordem anti-horária para a face inferior
  DrawTriangle(pA, pC, pB, color);
  DrawTriangle(pA, pD, pC, color);
}

void GameRenderer::drawTrapezoidLeft(Vector2 out_top_left, Vector2 out_bottom_right, Vector2 in_top_left, Vector2 in_bottom_right, Color color)
{
  // Define os 4 pontos da face esquerda
  Vector2 pA = {out_top_left.x, out_top_left.y};     // Superior Esquerdo Externo
  Vector2 pB = {out_top_left.x, out_bottom_right.y}; // Inferior Esquerdo Externo
  Vector2 pC = {in_top_left.x, in_bottom_right.y};   // Inferior Esquerdo Interno
  Vector2 pD = {in_top_left.x, in_top_left.y};       // Superior Esquerdo Interno

  // Ordem anti-horária para a face esquerda
  DrawTriangle(pA, pC, pB, color);
  DrawTriangle(pA, pD, pC, color);
}

void GameRenderer::drawTrapezoidRight(Vector2 out_top_left, Vector2 out_bottom_right, Vector2 in_top_left, Vector2 in_bottom_right, Color color)
{
  // Define os 4 pontos da face direita
  Vector2 pA = {out_bottom_right.x, out_top_left.y};     // Superior Direito Externo
  Vector2 pB = {in_bottom_right.x, in_top_left.y};       // Superior Direito Interno
  Vector2 pC = {in_bottom_right.x, in_bottom_right.y};   // Inferior Direito Interno
  Vector2 pD = {out_bottom_right.x, out_bottom_right.y}; // Inferior Direito Externo

  // Ordem anti-horária para a face direita
  DrawTriangle(pA, pC, pB, color);
  DrawTriangle(pA, pD, pC, color);
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