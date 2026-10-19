#pragma once

#include <raylib.h>
#include <game.hpp>

constexpr std::array PIECE_COLORS = {
    SKYBLUE,  // FILLED_I
    DARKBLUE, // FILLED_J
    ORANGE,   // FILLED_L
    YELLOW,   // FILLED_O
    GREEN,    // FILLED_S
    PURPLE,   // FILLED_T
    RED,      // FILLED_Z
};

// I - Sky Blue
#define I_HIGHLIGHT CLITERAL(Color){128, 224, 255, 255}
#define I_HALFTONE CLITERAL(Color){0, 153, 204, 255}
#define I_SHADOW CLITERAL(Color){0, 115, 153, 255}

// O - Yellow
#define O_HIGHLIGHT CLITERAL(Color){255, 240, 128, 255}
#define O_HALFTONE CLITERAL(Color){204, 172, 0, 255}
#define O_SHADOW CLITERAL(Color){153, 127, 0, 255}

// T - Purple
#define T_HIGHLIGHT CLITERAL(Color){208, 128, 255, 255}
#define T_HALFTONE CLITERAL(Color){128, 24, 192, 255}
#define T_SHADOW CLITERAL(Color){96, 16, 144, 255}

// S - Green
#define S_HIGHLIGHT CLITERAL(Color){128, 255, 169, 255}
#define S_HALFTONE CLITERAL(Color){0, 158, 66, 255}
#define S_SHADOW CLITERAL(Color){0, 107, 44, 255}

// Z - Red
#define Z_HIGHLIGHT CLITERAL(Color){255, 150, 147, 255}
#define Z_HALFTONE CLITERAL(Color){183, 45, 42, 255}
#define Z_SHADOW CLITERAL(Color){127, 31, 28, 255}

// J - Dark Blue
#define J_HIGHLIGHT CLITERAL(Color){102, 102, 205, 255}
#define J_HALFTONE CLITERAL(Color){0, 0, 111, 255}
#define J_SHADOW CLITERAL(Color){0, 0, 71, 255}

// L - Orange
#define L_HIGHLIGHT CLITERAL(Color){255, 192, 128, 255}
#define L_HALFTONE CLITERAL(Color){204, 112, 0, 255}
#define L_SHADOW CLITERAL(Color){153, 84, 0, 255}

constexpr int SCREEN_WIDTH = 1800;
constexpr int SCREEN_HEIGHT = 900;
constexpr int CELL_SIZE = 30;

constexpr Color BACKGROUND_COLOR = {23, 23, 46, 255};

constexpr Color SPAWN_COLOR = {41, 41, 65, 255};

constexpr Color SPAWN_HIGHLIGHT = {68, 68, 96, 255};
constexpr Color SPAWN_HALFTONE = {33, 33, 53, 255};
constexpr Color SPAWN_SHADOW = {20, 20, 35, 255};

constexpr Color BOARD_COLOR = {16, 16, 27, 255};

constexpr Color BOARD_HIGHLIGHT = {52, 52, 78, 255};
constexpr Color BOARD_HALFTONE = {28, 28, 45, 255};
constexpr Color BOARD_SHADOW = {8, 8, 17, 255};

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
  CellType pieceToCellType(CellState piece);

  void initWindow();

  void drawLoop();

  void drawClearCounts();

  void drawScoreText();

  void closeWindow();

  void drawBoard(const Game &game);
  void drawCell(int x, int y, Color base_color, Color shadow, Color halftone, Color highlight);
  void drawTrapezoidTop(Vector2 out_top_left, Vector2 out_bottom_right, Vector2 in_top_left, Vector2 in_bottom_right, Color color);
  void drawTrapezoidBottom(Vector2 out_top_left, Vector2 out_bottom_right, Vector2 in_top_left, Vector2 in_bottom_right, Color color);
  void drawTrapezoidLeft(Vector2 out_top_left, Vector2 out_bottom_right, Vector2 in_top_left, Vector2 in_bottom_right, Color color);
  void drawTrapezoidRight(Vector2 out_top_left, Vector2 out_bottom_right, Vector2 in_top_left, Vector2 in_bottom_right, Color color);

  void drawHoldedPiece();
  void drawPieceQueue();

  char pieceTypeToChar(PieceType piece);
};
