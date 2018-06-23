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
// Created by alik on 6/10/18.
//

#include "PlayerQueryBuilder.h"
#include "RawByteReader.h"
#include "RawByteBuilder.h"
#include "PlayersData.h"

#include <cassert>
#include <cmath>
#include <random>

PlayerQueryBuilder::PlayerQueryBuilder(const std::vector<uint8_t> &d) {
    this->loadFromRawByteData(d);
}


PlayerQueryBuilder &PlayerQueryBuilder::loadFromRawByteData(const std::vector<uint8_t> &d) {
    RawByteReader rr(d);
    uint8_t numPlayers = 0;
    rr.read_primitive(header_);
    rr.read_primitive(numPlayers);
    for (int i = 0; i < numPlayers; ++i) {
        auto data = fsq::make_shared<PlayersData>();
        rr.read_primitive(data->index_);
        rr.read_string(data->name_);
        rr.read_primitive(data->score_);
        rr.read_primitive(data->duration_);
        players_.push_back(data);
    }

    return *this;
}

std::vector<uint8_t> PlayerQueryBuilder::buildRawByteData() {
    RawByteBuilder bb;
    bb.push(header_);
    bb.push(static_cast<uint8_t>(players_.size()));
    for(auto &player : players_) {
        bb.push(player->index_)
          .push(player->name_)
          .push(player->score_)
          .push(player->duration_);
    }

    return bb.build();
}

PlayerQueryBuilder &PlayerQueryBuilder::push(const PlayersDataRef &data) {
    players_.push_back(data);
    return *this;
}

std::vector<PlayersDataRef> &PlayerQueryBuilder::players() {
    return players_;
}

PlayerQueryBuilder &PlayerQueryBuilder::push(const std::vector<PlayersDataRef> &data) {
    players_.reserve(players_.size() + data.size());
    std::copy(data.begin(), data.end(), std::back_inserter(players_));
    return *this;
}


PlayerQueryBuilder &PlayerQueryBuilder::incrementPlayersDeltaDuration(float amount, ONLY_CUSTOM_PLAYERS onlyCustomPlayers) {
    for(auto &player : players_) {
        if(onlyCustomPlayers == ONLY_CUSTOM_PLAYERS::YES && !player->isCustomPlayer()) {
            continue;
        }

        player->setDuration(player->getDuration() + amount);
    }
    return *this;
}

PlayerQueryBuilder &PlayerQueryBuilder::push(const PlayersData &data) {
    return push(fsq::make_shared<PlayersData>(data));
}
