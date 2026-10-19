#pragma once

#include <raylib.h>
#include <game.hpp>

constexpr Color SPAWN_COLOR = Color{75, 75, 75, 255};
constexpr Color BOARD_COLOR = Color{15, 15, 15, 255};
constexpr Color FILLED_COLOR = Color{100, 100, 100, 255};

constexpr std::array PIECE_COLORS = {
    SKYBLUE,  // FILLED_I
    DARKBLUE, // FILLED_J
    ORANGE,   // FILLED_L
    YELLOW,   // FILLED_O
    GREEN,    // FILLED_S
    PURPLE,   // FILLED_T
    RED,      // FILLED_Z
};

constexpr int SCREEN_WIDTH = 1800;
constexpr int SCREEN_HEIGHT = 900;
constexpr int CELL_SIZE = 30;

constexpr Color BACKGROUND_COLOR = Color{25, 25, 75, 255};

constexpr int OFFSET_X = SCREEN_WIDTH / 2 - 250;
constexpr int OFFSET_Y = SCREEN_HEIGHT / 2 - 300;

class GameRenderer
{

private:
  Game &game;

  std::array<std::array<CellState, BOARD_SIZE_Y>, BOARD_SIZE_X> rendering_board;

public:
  GameRenderer(Game &game);

  Color pieceToColor(CellState piece);

  void initWindow();

  void drawLoop();

  void drawClearCounts();

  void drawScoreText();

  void closeWindow();

  void drawBoard(const Game &game);
  void drawHoldedPiece();
  void drawPieceQueue();

  char pieceTypeToChar(PieceType piece);
};
