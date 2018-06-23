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

#ifndef FAKESERVERQUERIES_PLAYERQUERYBUILDER_H
#define FAKESERVERQUERIES_PLAYERQUERYBUILDER_H

#include <vector>
#include <cstdint>

#include "PlayersData.h"

class PlayerQueryBuilder {
public:
    explicit PlayerQueryBuilder(const std::vector<uint8_t> &d);
    PlayerQueryBuilder() = default;

    PlayerQueryBuilder &loadFromRawByteData(const std::vector<uint8_t> &d);
    std::vector<uint8_t> buildRawByteData();

    PlayerQueryBuilder &push(const PlayersData &data);
    PlayerQueryBuilder &push(const PlayersDataRef &data);
    PlayerQueryBuilder &push(const std::vector<PlayersDataRef> &data);

    enum class ONLY_CUSTOM_PLAYERS {
        YES,
        NO
    };

    PlayerQueryBuilder &incrementPlayersDeltaDuration(float amount, ONLY_CUSTOM_PLAYERS onlyCustomPlayers);
    std::vector<PlayersDataRef> &players();
private:
    std::vector<PlayersDataRef> players_;
    uint8_t header_ = 'D';
};


#endif //FAKESERVERQUERIES_PLAYERQUERYBUILDER_H
