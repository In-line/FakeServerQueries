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

#include "APIData.h"
#include <algorithm>
#include <functional>

int8_t APIData::getPlayersDiff() const {
    return playersDiff_;
}

void APIData::setPlayersDiff(int8_t playersDiff) {
    playersDiff_ = playersDiff;
}

std::size_t APIData::addPlayer(PlayersData &&player) {
    static std::size_t id = {};
    player.setIsCustomPlayer(true);
    players_[id] = fsq::make_shared<PlayersData>(player);
    return id++;
}

std::size_t APIData::getPlayersNum() const {
    return players_.size();
}

std::vector<PlayersDataRef> APIData::getPlayers() const {
    std::vector<PlayersDataRef> ret;
    ret.reserve(players_.size());

    for(const auto &el : players_) {
        ret.push_back(el.second);
    }
    return ret;
}

const PlayersDataRef &APIData::getPlayer(std::size_t id) {
    return players_.at(id);
}

PlayersDataRef APIData::deletePlayer(std::size_t id) {
    auto it = players_.find(id);
    if(it == players_.end()) {
        return {};
    } else {
        PlayersDataRef ref = it->second;
        players_.erase(it);
        return ref;
    }
}
