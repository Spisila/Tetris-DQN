
#include "piece_queue.hpp"
#include <types.hpp>


PieceQueue::~PieceQueue()
{
}

size_t PieceQueue::getQueueSize() const
{
    return queue_size;
}

// ! MULTIGAME RECIEVES SEED, PASSES IT TO EACH RLENV, PASSES IT TO EACH GAME, PASSES IT TO EACH QUEUE

const std::array<int, queue_size> PieceQueue::getPieceQueue() const
{
    std::array<int, queue_size> int_queue{};

    for (int i = 0; i < queue_size; i++)
    {
        int_queue.at(i) = static_cast<int>(known_piece_queue.at(i));
    }

    return int_queue;
}

int PieceQueue::getQueueIndex() const
{
    return queue_index;
}
// TODO: Variable queue size
void PieceQueue::generatePieceQueue()
{
    queue_index = 0;

    std::array<PieceType, 7> bag = {
        PieceType::I_PIECE_0,
        PieceType::J_PIECE_0,
        PieceType::L_PIECE_0,
        PieceType::O_PIECE_0,
        PieceType::S_PIECE_0,
        PieceType::T_PIECE_0,
        PieceType::Z_PIECE_0};

    std::shuffle(bag.begin(), bag.end(), gen);

    std::array<PieceType, 14> new_queue;

    for (int i = 0; i < 7; i++)
    {
        new_queue.at(i) = bag.at(i);
    }

    std::shuffle(bag.begin(), bag.end(), gen);

    for (int i = 7; i < 14; i++)
    {
        new_queue.at(i) = bag.at(i - 7);
    }

    for (int i = 0; i < 14; i++)
    {
        piece_queue.at(i) = new_queue.at(i);
    }
}

void PieceQueue::updatePieceQueue()
{
    std::array<PieceType, 7> bag = {
        PieceType::I_PIECE_0,
        PieceType::J_PIECE_0,
        PieceType::L_PIECE_0,
        PieceType::O_PIECE_0,
        PieceType::S_PIECE_0,
        PieceType::T_PIECE_0,
        PieceType::Z_PIECE_0};

    if (queue_index + 7 == piece_queue.size() + 1)
    {

        std::shuffle(bag.begin(), bag.end(), gen);

        for (int i = 0; i < 7; i++)
        {
            piece_queue.at(i) = bag.at(i);
        }
    }

    if (queue_index >= piece_queue.size() - 1)
    {
        std::shuffle(bag.begin(), bag.end(), gen);

        for (int i = 7; i < 14; i++)
        {
            piece_queue.at(i) = bag.at(i - 7);
        }

        queue_index = 0;
    }

    if (queue_index + 5 < 14)
    {
        known_piece_queue[0] = piece_queue[queue_index + 1];
        known_piece_queue[1] = piece_queue[queue_index + 2];
        known_piece_queue[2] = piece_queue[queue_index + 3];
        known_piece_queue[3] = piece_queue[queue_index + 4];
        known_piece_queue[4] = piece_queue[queue_index + 5];
    }
    else if (queue_index + 5 == 14)
    {
        known_piece_queue[0] = piece_queue[queue_index + 1];
        known_piece_queue[1] = piece_queue[queue_index + 2];
        known_piece_queue[2] = piece_queue[queue_index + 3];
        known_piece_queue[3] = piece_queue[queue_index + 4];
        known_piece_queue[4] = piece_queue[0];
    }
    else if (queue_index + 5 == 15)
    {
        known_piece_queue[0] = piece_queue[queue_index + 1];
        known_piece_queue[1] = piece_queue[queue_index + 2];
        known_piece_queue[2] = piece_queue[queue_index + 3];
        known_piece_queue[3] = piece_queue[0];
        known_piece_queue[4] = piece_queue[1];
    }
    else if (queue_index + 5 == 16)
    {
        known_piece_queue[0] = piece_queue[queue_index + 1];
        known_piece_queue[1] = piece_queue[queue_index + 2];
        known_piece_queue[2] = piece_queue[0];
        known_piece_queue[3] = piece_queue[1];
        known_piece_queue[4] = piece_queue[2];
    }
    else if (queue_index + 5 == 17)
    {
        known_piece_queue[0] = piece_queue[queue_index + 1];
        known_piece_queue[1] = piece_queue[0];
        known_piece_queue[2] = piece_queue[1];
        known_piece_queue[3] = piece_queue[2];
        known_piece_queue[4] = piece_queue[3];
    }

    queue_index++;
}