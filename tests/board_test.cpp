#include <gtest/gtest.h>
#include <board.hpp>
#include <game.hpp>

TEST(BoardTests, moveTetromino_validMove)
{
    Board b{};

    b.clearBoard();

    b.setPiece(PieceType::I_PIECE_0);

    EXPECT_TRUE(b.moveTetromino(MovementDirection::RIGHT));
}

TEST(BoardTests, moveTetromino_invalidMove)
{
    Board b{};

    b.clearBoard();

    b.setPiece(PieceType::I_PIECE_0);

    std::array<Position, 4> filled_spawn_position{
        Position{8, 1},
        Position{8, 2},
        Position{8, 3},
        Position{8, 4}};

    b.setTetrominoCellsStates(CellState::FILLED_I, filled_spawn_position);

    EXPECT_FALSE(b.moveTetromino(MovementDirection::RIGHT));
}

TEST(BoardTests, rotateTetromino_validMove)
{
    Board b{};

    b.clearBoard();

    b.setPiece(PieceType::S_PIECE_0);

    EXPECT_TRUE(b.rotateTetromino(Rotation::CLOCKWISE));
}

TEST(BoardTests, rotateTetromino_invalidMove)
{
    Board b{};

    b.clearBoard();

    b.setPiece(PieceType::S_PIECE_0);

    std::array<Position, 4> filled_spawn_position{
        Position{6, 1},
        Position{6, 2},
        Position{6, 3},
        Position{6, 4}};

    b.setTetrominoCellsStates(CellState::FILLED_I, filled_spawn_position);

    EXPECT_FALSE(b.rotateTetromino(Rotation::CLOCKWISE));
}

TEST(BoardTests, setPiece_returnFalse)
{

    Board b{};

    std::array<Position, 4> filled_spawn_position{
        Position{5, 1},
        Position{5, 2},
        Position{5, 3},
        Position{5, 4}};

    b.setTetrominoCellsStates(CellState::FILLED_I, filled_spawn_position);

    EXPECT_FALSE(b.setPiece(PieceType::I_PIECE_0));
}

TEST(BoardTests, setPiece_returnTrue)
{

    Board b{};

    std::array<Position, 4> clear_spawn_position{
        Position{5, 1},
        Position{5, 2},
        Position{5, 3},
        Position{5, 4}};

    b.setTetrominoCellsStates(CellState::EMPTY, clear_spawn_position);

    EXPECT_TRUE(b.setPiece(PieceType::I_PIECE_0));
}

TEST(BoardTests, checkOutOfLateralBounds_invalidPositions)
{
    Board b{};

    std::array<Position, 4> out_of_bounds_left{
        Position{-1, 5},
        Position{1, 5},
        Position{2, 4},
        Position{3, 4}};

    std::array<Position, 4> out_of_bounds_right{
        Position{BOARD_SIZE_X + 1, 15},
        Position{BOARD_SIZE_X + 2, 10},
        Position{8, 8},
        Position{7, 7}};

    std::array<Position, 4> out_of_bounds_down{
        Position{7, BOARD_SIZE_Y + 1},
        Position{6, BOARD_SIZE_Y + 1},
        Position{8, 15},
        Position{7, 15}};

    EXPECT_TRUE(b.checkOutOfLateralBounds(out_of_bounds_left));
    EXPECT_TRUE(b.checkOutOfLateralBounds(out_of_bounds_right));
    EXPECT_TRUE(b.checkOutOfLateralBounds(out_of_bounds_down));
}

TEST(BoardTests, checkOutOfLateralBounds_validPositions)
{

    Board b{};

    std::array<Position, 4> in_bounds_pos{
        Position{2, 1},
        Position{3, 2},
        Position{4, 3},
        Position{5, 4}};

    EXPECT_FALSE(b.checkOutOfLateralBounds(in_bounds_pos));
}

TEST(BoardTests, checkCollisions_validPositions)
{

    Board b{};

    std::array<Position, 4> in_bounds_pos{
        Position{2, 1},
        Position{3, 2},
        Position{4, 3},
        Position{5, 4}};

    for (Position pos : in_bounds_pos)
    {
        b.getBoardCells()[pos.x][pos.y] = CellState::EMPTY;
    }

    EXPECT_FALSE(b.checkOutOfLateralBounds(in_bounds_pos));
}

TEST(BoardTests, checkCollisions_invalidPositions)
{

    Board b{};

    std::array<Position, 4> in_bounds_pos{
        Position{2, 1},
        Position{3, 2},
        Position{4, 3},
        Position{5, 4}};

    b.getBoardCells()[in_bounds_pos[0].x][in_bounds_pos[0].y] = CellState::FILLED_I;

    EXPECT_FALSE(b.checkOutOfLateralBounds(in_bounds_pos));
}

TEST(BoardTests, chechTouchedFloor_validPositions)
{

    Board b{};

    std::array<Position, 4> in_bounds_pos{
        Position{2, 1},
        Position{3, 2},
        Position{4, 3},
        Position{5, 4}};

    EXPECT_FALSE(b.checkTouchedFloor(in_bounds_pos));
}

TEST(BoardTests, chechTouchedFloor_invalidPositions)
{

    Board b{};

    std::array<Position, 4> out_of_bounds_pos{
        Position{2, BOARD_SIZE_Y + 1},
        Position{3, 2},
        Position{4, 3},
        Position{5, 4}};

    EXPECT_TRUE(b.checkTouchedFloor(out_of_bounds_pos));
}

TEST(BoardTests, clearLines_LinesCleared)
{

    Board b{};
    for (int line = BOARD_SIZE_Y - 4; line < BOARD_SIZE_Y; line++)
    {
        for (int column = 0; column < 8; column += 4)
        {
            std::array<Position, 4> positions{
                Position{column, line},
                Position{column + 1, line},
                Position{column + 2, line},
                Position{column + 3, line}};
            b.setTetrominoCellsStates(CellState::FILLED_I, positions);
        }

        std::array<Position, 4> remaining_positions{
            Position{8, line},
            Position{9, line},
            Position{8, line},
            Position{9, line}};
        b.setTetrominoCellsStates(CellState::FILLED_I, remaining_positions);
    }

    EXPECT_EQ(4, b.clearLines());
}

TEST(BoardTests, clearLines_noLinesCleared)
{

    Board b{};

    b.clearBoard();

    EXPECT_EQ(0, b.clearLines());
}

TEST(BoardTests, name)
{
}