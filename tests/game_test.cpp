#include <gtest/gtest.h>
#include <tetris.hpp>

class GameTests : public ::testing::Test
{
protected:
    Game game{0, 1};
};

TEST_F(GameTests, constructorStoresId)
{
    EXPECT_EQ(game.getId(), 0);
}

TEST_F(GameTests, scoreStartsAtZero)
{
    EXPECT_EQ(game.getScore(), 0);
    EXPECT_EQ(game.getClearedLines(), 0);
}

TEST_F(GameTests, clearCountsStartAtZero)
{
    EXPECT_EQ(game.getClearCounts(), (std::array<unsigned int, 4>{0, 0, 0, 0}));
}

TEST_F(GameTests, increaseScoreTracksSingleLineClear)
{
    game.increaseScore(1);

    EXPECT_EQ(game.getScore(), 100);
    EXPECT_EQ(game.getClearedLines(), 1);
    EXPECT_EQ(game.getClearCounts(), (std::array<unsigned int, 4>{1, 0, 0, 0}));
}

TEST_F(GameTests, increaseScoreTracksTetrisClear)
{
    game.increaseScore(4);

    EXPECT_EQ(game.getScore(), 1000);
    EXPECT_EQ(game.getClearedLines(), 4);
    EXPECT_EQ(game.getClearCounts(), (std::array<unsigned int, 4>{0, 0, 0, 1}));
}

TEST_F(GameTests, resetScoreClearsScoreAndCounts)
{
    game.increaseScore(1);
    game.increaseScore(4);

    game.resetScore();

    EXPECT_EQ(game.getScore(), 0);
    EXPECT_EQ(game.getClearCounts(), (std::array<unsigned int, 4>{0, 0, 0, 0}));
    EXPECT_EQ(game.getClearedLines(), 5);
}

TEST_F(GameTests, hardDropPlacesPieceAndSpawnsNextPiece)
{
    game.hardDrop();

    EXPECT_TRUE(game.piece_set);
    EXPECT_EQ(game.getBoard().getAggregateHeight(), 4);
    EXPECT_EQ(game.getScore(), 0);
}

TEST_F(GameTests, holdStoresCurrentPieceAndUsesQueuedPiece)
{
    const int current_piece = game.getBoard().getBoardTetromino().getCurrentPieceType();
    const int queued_piece = game.getPieceQueue().getPieceQueue().at(0);

    game.holdCurrentPiece();

    EXPECT_EQ(game.hold_piece_index, current_piece);
    EXPECT_EQ(game.getBoard().getBoardTetromino().getCurrentPieceType(), queued_piece);
}

TEST_F(GameTests, gameLostStoresHighScoreAndResetsCurrentScore)
{
    game.increaseScore(1);

    game.gameLost();

    EXPECT_TRUE(game.lost);
    EXPECT_EQ(game.getHighScore(), 100);
    EXPECT_EQ(game.getScore(), 0);
    EXPECT_EQ(game.getHighClearCounts(), (std::array<unsigned int, 4>{1, 0, 0, 0}));
}
