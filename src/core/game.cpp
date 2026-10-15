
#include <game.hpp>

#include <raylib.h>

#include <iostream>
#include <algorithm>
#include <iterator>

/* TODO:
Implement wall kicks
*/

Game::Game(int _id)
{

    id = _id;

    piece_queue.generatePieceQueue();
    piece_queue.updatePieceQueue();
    game_board.setPiece(static_cast<PieceType>(piece_queue.getPieceQueue().at(0)));
}

Game::~Game()
{
}

#pragma region GETTERS

int Game::getId() const
{
    return id;
}

int Game::getScore()
{
    return score;
}

PieceQueue &Game::getPieceQueue()
{
    return piece_queue;
}

const PieceQueue &Game::getPieceQueue() const
{
    return piece_queue;
}

Board &Game::getBoard()
{
    return game_board;
}

const Board &Game::getBoard() const
{
    return game_board;
}

std::array<Position, 4> Game::getLastPiecePlacedPositions() const
{
    return last_piece_placed_positions;
}

#pragma endregion

#pragma region SETTERS

void Game::resetScore()
{
    score = 0;
}

#pragma endregion

void Game::hardDrop()
{

    Tetromino &t = game_board.getBoardTetromino();

    int counter = 0;

    while (true)
    {
        game_board.setTetrominoCellsStates(CellState::EMPTY, t.getAllPositions());
        auto projected_position = t.projectMovement(MovementDirection::DOWN);

        if (game_board.checkShouldSetPiece(projected_position))
        {
            increaseScore(pieceWasSet());
            break;
        }

        t.setPositions(projected_position);
        game_board.setTetrominoCellsStates(CellState::ACTIVE, projected_position);
    }
}

void Game::increaseScore(int lines)
{

    lines_cleared += lines;

    switch (lines)
    {
    case 0:
        break;
    case 1:
        score += 100;
        break;
    case 2:
        score += 200;
        break;
    case 3:
        score += 400;
        break;
    case 4:
        score += 1000;
        break;

    default:
        break;
    }
}

int Game::getClearedLines()
{
    return lines_cleared;
}

int Game::pieceWasSet()
{

    auto tetromino_positions = game_board.getBoardTetromino().getAllPositions();

    auto next_piece_in_queue = static_cast<PieceType>(piece_queue.getPieceQueue().at(0));

    game_board.setTetrominoCellsStates(CellState::FILLED, tetromino_positions);

    std::copy(std::begin(tetromino_positions), std::end(tetromino_positions), std::begin(last_piece_placed_positions));

    last_landing_height = game_board.calculateLandingHeight(tetromino_positions);
    last_eroded_piece_cells = game_board.calculateErodedPieceCells(tetromino_positions);

    if (!game_board.setPiece(next_piece_in_queue))
    {
        gameLost();
        return 0;
    }

    piece_set = true;
    hold_used = false;
    piece_queue.updatePieceQueue();

    int lines = game_board.clearLines();
    lines_cleared_last = lines;

    return lines;
}

float Game::getLastLandingHeight() const
{
    return last_landing_height;
}

int Game::getLastErodedPieceCells() const
{
    return last_eroded_piece_cells;
}

int Game::getLinesClearedLast() const
{
    return lines_cleared_last;
}

void Game::holdCurrentPiece()
{
    game_board.setTetrominoCellsStates(CellState::EMPTY, game_board.getBoardTetromino().getAllPositions());

    Tetromino &t = game_board.getBoardTetromino();

    int current_index = static_cast<int>(t.getCurrentPieceType());

    if (hold_piece_index == -1)
    {
        hold_piece_index = static_cast<int>(current_index);
        game_board.setPiece(static_cast<PieceType>(getPieceQueue().getPieceQueue().at(0))); // Spawn new random piece
    }
    else
    {
        int temp = hold_piece_index;
        hold_piece_index = static_cast<int>(current_index);
        game_board.setPiece(static_cast<PieceType>(temp));
    }
}

void Game::setGravity(bool on)
{
    gravity_on = on;
}

void Game::gameLost()
{
    lost = true;
    game_board.clearBoard();
    piece_queue.generatePieceQueue();
    piece_queue.updatePieceQueue();
    game_board.setPiece(static_cast<PieceType>(getPieceQueue().getPieceQueue().at(0)));
    game_board.setTetrominoCellsStates(CellState::ACTIVE, game_board.getBoardTetromino().getAllPositions());
}

// TODO: Make gravity work
void Game::tickGravity()
{

    gravity_counter++;

    if (gravity_counter >= gravity_ticks)
    {
        gravity_counter = 0;

        auto tetromino = game_board.getBoardTetromino();

        game_board.setTetrominoCellsStates(CellState::EMPTY, tetromino.getAllPositions());

        std::array<Position, 4> projected_position = tetromino.projectMovement(MovementDirection::DOWN);

        if (game_board.checkShouldSetPiece(projected_position))
        {
            pieceWasSet();
            return;
        }

        tetromino.setPositions(projected_position);

        game_board.setTetrominoCellsStates(CellState::ACTIVE, tetromino.getAllPositions());
    }
}