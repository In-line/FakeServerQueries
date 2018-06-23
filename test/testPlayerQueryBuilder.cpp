//
// Created by alik on 6/13/18.
//

#include <gtest/gtest.h>
#include <PlayerQueryBuilder.h>

TEST(testPlayerQueryBuilder, main)
{
    auto v1 = PlayerQueryBuilder()
            .push(
                    PlayersData()
                    .setName("Test1")
                    .setScore(1)
            )
            .push(
                    PlayersData()
                    .setName("Test2")
                    .setScore(2)
            )
            .buildRawByteData();

    auto v2 = PlayerQueryBuilder(v1)
            .buildRawByteData();

    ASSERT_EQ(v1, v2);


    auto players = PlayerQueryBuilder(v1).players();
    ASSERT_EQ(players.size(), 2);
    ASSERT_EQ(players[0]->getName(), "Test1");
    ASSERT_EQ(players[1]->getName(), "Test2");
}