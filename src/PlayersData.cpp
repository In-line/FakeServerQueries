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

#include "PlayersData.h"

uint8_t PlayersData::getIndex() const {
    return index_;
}

PlayersData &PlayersData::setIndex(uint8_t index) {
    index_ = index;
    return *this;
}

const std::string &PlayersData::getName() const {
    return name_;
}

PlayersData &PlayersData::setName(const std::string &name) {
    name_ = name;
    return *this;
}

uint32_t PlayersData::getScore() const {
    return score_;
}

PlayersData &PlayersData::setScore(uint32_t score) {
    score_ = score;
    return *this;
}

float PlayersData::getDuration() const {
    return duration_;
}

PlayersData &PlayersData::setDuration(float duration) {
    duration_ = duration;
    return *this;
}

bool PlayersData::isCustomPlayer() const {
    return isCustomPlayer_;
}

void PlayersData::setIsCustomPlayer(bool isCustomPlayer) {
    isCustomPlayer_ = isCustomPlayer;
}
