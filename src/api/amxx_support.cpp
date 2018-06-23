/*
 * 
 * Copyright (c) 2018 Alik Aslanyan <cplusplus256@gmail.com>
 *
 *
 *    This file is part of Fake Server Queries
 *
 *    Fake Server Queries is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    Fake Server Queries is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Fake Server Queries; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of Fake Server Queries with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */

//
// Created by alik on 6/15/18.
//

#include "amxx_support.h"
#include <SendToHooker.h>
#include <amxxmodule.h>
#include <random>
#include <memory>
#include <boost/functional/hash.hpp>

std::unique_ptr<APIData> g_APIData;

#define CHECK_FAKE_ID(VALUE, RETURN_VALUE) \
    do { \
        if((VALUE) < 0) { \
            MF_LogError(amx, AMX_ERR_NATIVE, "Fake id %d can't be negative", (VALUE)); \
            return (RETURN_VALUE); \
        } \
    } while(false)

auto generateRandomDurationIncrement() {
    static std::unique_ptr<std::mt19937> rg = {};
    if(!rg) {
        std::random_device rd;
        rg = std::make_unique<std::mt19937>(rd());
    }

    std::uniform_real_distribution<float> distribution(0, 4);
    return distribution(*rg);
}

static cell AMX_NATIVE_CALL fsq_set_players_diff(AMX *amx, cell *params) /* 1 param */
{
    int playersDiff = params[1];
    if( std::abs(playersDiff) > std::numeric_limits<int8_t>::max() ) {
        MF_LogError(amx, AMX_ERR_NATIVE, "Players diff %d overflow", playersDiff);
        return -1;
    } else {
        g_APIData->setPlayersDiff(int8_t (playersDiff));
        return 0;
    }
}

// (name[]. score, Float:duration, random_duration_increment)
static cell AMX_NATIVE_CALL fsq_create_fake(AMX *amx, cell *params)
{
    int score = params[2];
    if(score < 0) {
        MF_LogError(amx, AMX_ERR_NATIVE, "Score %d can't be negative integer", score);
        return -1;
    }

    int nameLen = 0;
    char *name = MF_GetAmxString(amx, params[1], 0, &nameLen);

    float duration = amx_ctof(params[3]);
    if(duration < 0) {
        MF_LogError(amx, AMX_ERR_NATIVE, "Duration %f can't be negative number", duration);
        return -1;
    }

    if(params[4] != 0) {
        duration += generateRandomDurationIncrement();
    }

    return g_APIData->addPlayer(std::move(
            PlayersData()
                    .setName(std::string(name, nameLen))
                    .setScore(static_cast<uint32_t>(score))
                    .setDuration(duration)
    ));
}

static cell AMX_NATIVE_CALL fsq_destroy_fake(AMX *amx, cell *params) {
    auto id = params[1];
    CHECK_FAKE_ID(id, -1);
    if(g_APIData->deletePlayer(static_cast<std::size_t>(id))) {
        return 0;
    } else {
        return -1;
    }
}

// (id, Float:duration, random_duration_increment = true)
static cell AMX_NATIVE_CALL fsq_set_fake_duration(AMX *amx, cell *params) {
    auto id = params[1];
    CHECK_FAKE_ID(id, -1);
    try {
        auto duration = amx_ctof(params[2]);
        if(duration < 0.0) {
            MF_LogError(amx, AMX_ERR_NATIVE, "Duration %f can't be negative", duration);
            return -1;
        }

        if(params[3] != 0) {
            duration += generateRandomDurationIncrement();
        }
        g_APIData->getPlayer(static_cast<std::size_t>(id))->setDuration(duration);
        return duration;
    } catch(std::out_of_range&) {
        MF_LogError(amx, AMX_ERR_NATIVE, "Player %d was not found", id);
        return amx_ftoc(-1);
    }
}

// (id)
static cell AMX_NATIVE_CALL fsq_get_fake_duration(AMX *amx, cell *params) {
    auto id = params[1];
    CHECK_FAKE_ID(id, amx_ftoc(-1));

    try {
        return amx_ftoc(g_APIData->getPlayer(static_cast<std::size_t>(id))->getDuration());
    } catch(std::out_of_range&) {
        MF_LogError(amx, AMX_ERR_NATIVE, "Player %d was not found", id);
        return amx_ftoc(-1);
    }
}

// (id)
static cell AMX_NATIVE_CALL fsq_get_fake_score(AMX *amx, cell *params) {
    auto id = params[1];
    CHECK_FAKE_ID(id, -1);

    try {
        return g_APIData->getPlayer(static_cast<std::size_t>(id))->getScore();
    } catch(std::out_of_range&) {
        MF_LogError(amx, AMX_ERR_NATIVE, "Player %d was not found", id);
        return amx_ftoc(-1);
    }
}

static cell AMX_NATIVE_CALL fsq_set_fake_score(AMX *amx, cell *params) {
    auto id = params[1];
    CHECK_FAKE_ID(id, -1);

    try {
        auto player = g_APIData->getPlayer(static_cast<std::size_t>(id));
        player->setScore(params[2]);
        return 0;
    } catch(std::out_of_range&) {
        MF_LogError(amx, AMX_ERR_NATIVE, "Player %d was not found", id);
        return -1;
    }
}

DLLEXPORT void OnAmxxAttach()
{
    try {
        SendToHooker::initInstance();
    } catch(std::exception &e) {
        MF_Log("C++ exception occurred during sendto hooker initialization.");
        MF_Log("%s", e.what());
        return;
    }

    static AMX_NATIVE_INFO module_exports[] = {
            {"fsq_set_players_diff", fsq_set_players_diff},
            {"fsq_create_fake", fsq_create_fake},
            {"fsq_destroy_fake", fsq_destroy_fake},
            {"fsq_set_fake_duration", fsq_set_fake_duration},
            {"fsq_get_fake_duration", fsq_get_fake_duration},
            {"fsq_set_fake_score", fsq_set_fake_score},
            {"fsq_get_fake_score", fsq_get_fake_score},
            {nullptr, nullptr}
    };

    g_APIData = std::make_unique<APIData>();
    MF_AddNatives(module_exports);
}

DLLEXPORT void OnPluginsUnloaded() {
    g_APIData = std::make_unique<APIData>();
}

DLLEXPORT void OnAmxxDetach()
{
    SendToHooker::freeInstance();
    g_APIData = {};
}