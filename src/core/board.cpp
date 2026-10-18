#include <board.hpp>
#include <shared.hpp>
#include <iostream>
#include <cassert>

Board::Board(/* args */)
{

    // initialize the board as empty
    for (int x = 0; x < board.size(); x++)
    {
        for (int y = 0; y < board.at(x).size(); y++)
        {
            board.at(x).at(y) = CellState::EMPTY;
        }
    }
}

Board::~Board()
{
}

std::array<std::array<CellState, BOARD_SIZE_Y>, BOARD_SIZE_X> Board::getBoardCells() const
{
    return board;
}

std::array<int, BOARD_SIZE_X> Board::getBoardHeight() const
{

    std::array<int, BOARD_SIZE_X> heights{};

    for (int x = 0; x < board.size(); x++)
    {
        for (int y = 0; y < board.at(x).size(); y++)
        {
            if (board.at(x).at(y) == CellState::FILLED)
            {
                heights.at(x)++;
            }
        }
    }

    return heights;
}

int Board::getAggregateHeight() const
{

    auto columns_heights = getBoardHeight();

    int total_height = 0;

    for (int i = 0; i < columns_heights.size(); i++)
    {
        total_height += columns_heights.at(i);
    }

    return total_height;
}

std::array<int, BOARD_SIZE_X> Board::getColumnTopHeights() const
{
    std::array<int, BOARD_SIZE_X> heights{};

    for (int x = 0; x < BOARD_SIZE_X; x++)
    {
        for (int y = 0; y < BOARD_SIZE_Y; y++)
        {
            if (board[x][y] == CellState::FILLED)
            {
                heights[x] = BOARD_SIZE_Y - y; // primeira célula preenchida de cima para baixo
                break;
            }
        }
    }
    return heights;
}

std::array<int, BOARD_SIZE_X - 1> Board::getColumnHeightDifferences() const
{

    auto heights = getColumnTopHeights();
    std::array<int, BOARD_SIZE_X - 1> differences{};

    for (int x = 0; x < BOARD_SIZE_X - 1; x++)
    {
        differences[x] = heights[x + 1] - heights[x];
    }

    return differences;
}

int Board::getAmountOfHoles() const
{

    int holes = 0;

    for (int x = 0; x < board.size(); x++)
    {
        for (int y = 0; y < board.at(x).size(); y++)
        {

            // If is the first line there can be no empty cell above it
            if (y == 0)
            {
                continue;
            }

            auto cell_i = board.at(x).at(y);
            auto cell_above = board.at(x).at(y - 1);

            if (cell_i == CellState::EMPTY)
            {

                if (cell_above == CellState::FILLED)
                {
                    holes++;
                }
            }
        }
    }

    return holes;
}

int Board::getRugosity() const
{

    auto column_heights = getBoardHeight();

    int rugosity = 0;

    for (int i = 0; i < column_heights.size() - 1; i++)
    {

        int difference = 0;

        difference = column_heights.at(i) - column_heights.at(i + 1);

        rugosity += std::abs(difference);
    }

    return rugosity;
}

int Board::getRowTransitions() const
{
    int transitions = 0;

    for (int y = 0; y < BOARD_SIZE_Y; y++)
    {
        CellState previous = CellState::FILLED; // parede esquerda conta como preenchida

        for (int x = 0; x < BOARD_SIZE_X; x++)
        {
            bool current_filled = (board[x][y] == CellState::FILLED);
            bool previous_filled = (previous == CellState::FILLED);

            if (current_filled != previous_filled)
            {
                transitions++;
            }

            previous = board[x][y];
        }

        if (previous != CellState::FILLED) // parede direita conta como preenchida
        {
            transitions++;
        }
    }

    return transitions;
}

int Board::getColumnTransitions() const
{
    int transitions = 0;

    for (int x = 0; x < BOARD_SIZE_X; x++)
    {
        CellState previous = CellState::EMPTY; // nada acima da primeira linha

        for (int y = 0; y < BOARD_SIZE_Y; y++)
        {
            bool current_filled = (board[x][y] == CellState::FILLED);
            bool previous_filled = (previous == CellState::FILLED);

            if (current_filled != previous_filled)
            {
                transitions++;
            }

            previous = board[x][y];
        }

        if (previous != CellState::FILLED) // chão conta como preenchido
        {
            transitions++;
        }
    }

    return transitions;
}

int Board::getWellSums() const
{
    int well_sum = 0;

    for (int x = 0; x < BOARD_SIZE_X; x++)
    {
        int well_depth = 0;

        for (int y = 0; y < BOARD_SIZE_Y; y++)
        {
            bool is_empty = (board[x][y] != CellState::FILLED);
            bool left_filled = (x == 0) ? true : (board[x - 1][y] == CellState::FILLED);
            bool right_filled = (x == BOARD_SIZE_X - 1) ? true : (board[x + 1][y] == CellState::FILLED);

            if (is_empty && left_filled && right_filled)
            {
                well_depth++;
                well_sum += well_depth;
            }
            else
            {
                well_depth = 0;
            }
        }
    }

    return well_sum;
}

float Board::calculateLandingHeight(const std::array<Position, 4> &piece_positions) const
{
    float avg_y = 0.0f;

    for (const Position &pos : piece_positions)
    {
        avg_y += static_cast<float>(pos.y);
    }

    avg_y /= static_cast<float>(piece_positions.size());

    return static_cast<float>(BOARD_SIZE_Y - 1) - avg_y;
}

int Board::calculateErodedPieceCells(const std::array<Position, 4> &piece_positions) const
{
    std::array<int, 4> touched_rows{};
    int touched_count = 0;

    for (const Position &pos : piece_positions)
    {
        bool already_seen = false;

        for (int i = 0; i < touched_count; i++)
        {
            if (touched_rows[i] == pos.y)
            {
                already_seen = true;
                break;
            }
        }

        if (!already_seen)
        {
            touched_rows[touched_count++] = pos.y;
        }
    }

    int rows_cleared = 0;
    int piece_cells_eliminated = 0;

    for (int i = 0; i < touched_count; i++)
    {
        int row = touched_rows[i];
        bool row_full = true;

        for (int x = 0; x < BOARD_SIZE_X; x++)
        {
            if (board[x][row] != CellState::FILLED)
            {
                row_full = false;
                break;
            }
        }

        if (!row_full)
        {
            continue;
        }

        rows_cleared++;

        for (const Position &pos : piece_positions)
        {
            if (pos.y == row)
            {
                piece_cells_eliminated++;
            }
        }
    }

    return rows_cleared * piece_cells_eliminated;
}

const std::array<int, 4> Board::getBoardState() const
{

    int row_transitions = getRowTransitions();
    int column_transitions = getColumnTransitions();
    int holes = getAmountOfHoles();
    int wells = getWellSums();

    std::array<int, 4> data_basic = {
        row_transitions,
        column_transitions,
        holes,
        wells};

    return data_basic;
}

const Tetromino &Board::getBoardTetromino() const
{

    return tetromino;
}

Tetromino &Board::getBoardTetromino()
{
    return tetromino;
}

void Board::setTetrominoCellsStates(CellState state, std::array<Position, 4> positions)
{
    for (Position pos : positions)
    {
        board[pos.x][pos.y] = state;
    }
}

bool Board::moveTetromino(MovementDirection dir)
{
    std::array<Position, 4> projected_position = tetromino.projectMovement(dir);

    if (checkOutOfLateralBounds(projected_position) || checkCollisions(projected_position))
    {
        return false;
    }

    setTetrominoCellsStates(CellState::EMPTY, tetromino.getAllPositions());

    tetromino.setPositions(projected_position);

    setTetrominoCellsStates(CellState::ACTIVE, projected_position);

    return true;
}

void Board::moveTetrominoToColumn(int column)
{

    assert(column > -1 && "Cannot move tetromino outside board");
    assert(column < BOARD_SIZE_X && "Cannot move tetromino outside board");

    int spawn_x_position = spawn_positions.at(0).x;

    if (column == spawn_x_position)
    {
        return;
    }

    if (column < spawn_x_position)
    {

        for (int i = 0; i < spawn_x_position - column; i++)
        {
            moveTetromino(MovementDirection::LEFT);
        }
        return;
    }

    if (column > spawn_x_position)
    {
        for (int i = 0; i < column - spawn_x_position; i++)
        {
            moveTetromino(MovementDirection::RIGHT);
        }
        return;
    }
}

bool Board::rotateTetromino(Rotation rot)
{

    std::array<Position, 4> projected_rotation = tetromino.projectRotation(rot);

    if (checkOutOfLateralBounds(projected_rotation) || checkCollisions(projected_rotation))
    {
        return false;
    }

    setTetrominoCellsStates(CellState::EMPTY, tetromino.getAllPositions());

    tetromino.setPositions(projected_rotation);

    tetromino.setTetrominoType(tetromino.changeTetrominoeRotation(rot));

    setTetrominoCellsStates(CellState::ACTIVE, projected_rotation);

    return true;
}

void Board::rotateTetrominoToRotation(int rotation)
{

    assert(rotation > -1 && "Rotation must be between 0 and 3");
    assert(rotation < 4 && "Rotation must be between 0 and 3");

    int fail_counter = 0;

    // if (rotation == 0) {
    //     return;
    // }

    for (int i = 0; i < rotation; i++)
    {
        rotateTetromino(Rotation::CLOCKWISE);
    }

    assert(fail_counter < 10 && "Failed to rotate piece to rotation");
}

void Board::moveTetrominoToColumnWithRotation(int column, int rotation)
{
    rotateTetrominoToRotation(rotation);
    moveTetrominoToColumn(column);
}

bool Board::setPiece(PieceType new_piece)
{

    auto new_piece_offsets = rotationToPositionOffsets.at(new_piece);

    Position new_pivot_position = spawn_positions[0];

    std::array<Position, 4> new_piece_positions = {
        new_pivot_position,
        new_pivot_position,
        new_pivot_position,
        new_pivot_position};

    // Apply offsets
    for (int j = 0; j < new_piece_offsets.size(); j++)
    {
        new_piece_positions[j + 1].x += new_piece_offsets[j].x;
        new_piece_positions[j + 1].y += new_piece_offsets[j].y;
    }

    if (checkCollisions(new_piece_positions))
    {
        return false;
    }

    tetromino.setPositions(new_piece_positions);

    tetromino.setTetrominoType(new_piece);

    setTetrominoCellsStates(CellState::ACTIVE, tetromino.getAllPositions());

    return true;
}

bool Board::checkCollisions(std::array<Position, 4> projected_positions)
{

    if (checkOutOfLateralBounds(projected_positions))
    {
        return true;
    }

    for (Position pos : projected_positions)
    {
        if (board[pos.x][pos.y] == CellState::FILLED)
        {
            return true;
        }
    }

    return false;
}

bool Board::checkOutOfLateralBounds(std::array<Position, 4> projected_positions)
{

    for (Position pos : projected_positions)
    {
        if (pos.x < 0 || pos.x >= BOARD_SIZE_X || pos.y < 0 || pos.y >= BOARD_SIZE_Y)
        {
            return true;
        }
    }

    return false;
}

bool Board::checkTouchedFloor(std::array<Position, 4> projected_positions)
{

    for (Position pos : projected_positions)
    {
        if (pos.y >= BOARD_SIZE_Y)
        {
            return true;
        }
    }

    return false;
}

bool Board::checkShouldSetPiece(std::array<Position, 4> projected_position)
{
    bool test_floor = checkTouchedFloor(projected_position);
    bool test_collision = checkCollisions(projected_position);

    return test_collision || test_floor;
}

int Board::clearLines()
{
    int cleared_lines = 0;

    // Loop through all lines and count filled cells
    for (int line = 0; line < BOARD_SIZE_Y; line++)
    {
        int filled_count = 0;

        // Check if line full
        for (int x = 0; x < BOARD_SIZE_X; x++)
        {

            if (board[x][line] == CellState::FILLED)
            {
                filled_count++;
            }
        }

        // If line is full, clear it and move everything above down

        if (filled_count == BOARD_SIZE_X)
        {
            cleared_lines++;

            int cleared_line = line;
            for (int line_above = line - 1; line_above > 0; line_above--)
            {
                for (int cell = 0; cell < BOARD_SIZE_X; cell++)
                {

                    if (board[cell][line_above] != CellState::ACTIVE)
                    {
                        board[cell][cleared_line] = board[cell][line_above];
                    }
                }
                cleared_line = line_above;
            }
        }
    }
    return cleared_lines;
}

void Board::clearBoard()
{

    for (int x = 0; x < board.size(); x++)
    {
        for (int y = 0; y < board[x].size(); y++)
        {
            if (board[x][y] == CellState::FILLED)
            {
                board[x][y] = CellState::EMPTY;
            }
        }
    }
}