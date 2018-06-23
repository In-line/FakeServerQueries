//
// Created by alik on 5/20/18.
//
#include <iostream>

#include <gtest/gtest.h>
#include <InfoQueryBuilder.h>
#include <ModInfoObsoleteBuilder.h>

TEST(testInfoQueryBuilder, main)
{
    auto v1 = InfoQueryBuilder()
            .setHeader(InfoQueryBuilder::HEADER_FORMAT::OBSOLETE)
            .setName("NAME")
            .setFolder("FOLDER")
            .setGame("GAME")
            .setEnvironment(InfoQueryBuilder::ENVIRONMENT::MAC)
            .setVac(InfoQueryBuilder::VAC::SECURED)
            .setEnvironment(InfoQueryBuilder::ENVIRONMENT::LINUX)
            .setBots(15)
            .setModInfoObsolete(
                    ModInfoObsoleteBuilder()
                            .setLink("LINK")
                            .setDownloadLink("DOWNLOADLINK")
                            .build(ModInfoObsolete::MOD::HL_MOD)
            )
            .buildRawByteData();

    auto v2 = InfoQueryBuilder()
            .loadFromRawByteData(v1)
            .buildRawByteData();

    EXPECT_EQ(v1, v2);
}

TEST(testInfoQueryBuilder, empty)
{
    auto v1 = InfoQueryBuilder()
            .setHeader(InfoQueryBuilder::HEADER_FORMAT::OBSOLETE)
            .setModInfoObsolete(
                    ModInfoObsoleteBuilder()
                    .build(ModInfoObsolete::MOD::HL_MOD)
            )
            .buildRawByteData();

    auto v2 = InfoQueryBuilder()
            .loadFromRawByteData(v1)
            .buildRawByteData();

    EXPECT_EQ(v1, v2);
}