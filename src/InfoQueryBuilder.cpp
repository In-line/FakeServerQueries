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
// Created by alik on 5/19/18.
//

#include "InfoQueryBuilder.h"
#include "ModInfoObsoleteBuilder.h"
#include "RawByteBuilder.h"
#include "RawByteReader.h"

InfoQueryBuilder::InfoQueryBuilder(const std::vector<uint8_t> &v) {
    this->loadFromRawByteData(v);
}

std::vector<uint8_t> InfoQueryBuilder::buildRawByteData() {
    RawByteBuilder rb;

    rb.push(isObsolete() ? 'm' : 'I');
    if(isObsolete()) rb.push(address_);
    if(!isObsolete()) rb.push(protocol_);
    rb.push(name_);
    rb.push(map_);
    rb.push(folder_);
    rb.push(game_);
    if(!isObsolete()) rb.push(id_);
    rb.push(players_);
    rb.push(maxPlayers_);
    if(isObsolete()) rb.push(protocol_);
    if(!isObsolete()) rb.push(bots_);
    rb.push(serverTypeToByte(serverType_));
    rb.push(environmentToByte(environment_));
    rb.push<uint8_t>(visibility_ == VISIBILITY::PUBLIC ? 0 : 1);
    if(isObsolete()) rb.push(modInfoObsolete_.as_vector());
    rb.push<uint8_t>(vac_ == VAC::SECURED ? 1 : 0);
    if(isObsolete()) rb.push(bots_);
    if(!isObsolete()) rb.push(version_);
    rb.push(additionalData_);

    return std::move(rb.build());
}

uint8_t InfoQueryBuilder::serverTypeToByte(SERVER_TYPE serverType) const {
    switch(serverType) {
        case SERVER_TYPE::DEDICATED:
            return (isObsolete() ? 'D' : 'd');
        case SERVER_TYPE::NON_DEDICATED:
            return (isObsolete() ? 'L' : 'l');
        case SERVER_TYPE::TV:
            return (isObsolete() ? 'L' : 'p');
    }
    return {};
}

InfoQueryBuilder::SERVER_TYPE InfoQueryBuilder::byteToServerType(uint8_t byte) const {
    switch (std::tolower(byte)) {
        case 'd':
            return SERVER_TYPE::DEDICATED;
        case 'l':
            return SERVER_TYPE::NON_DEDICATED;
        case 'p':
            return SERVER_TYPE::TV;
        default:
            throw InvalidPacketException();
    }
}

uint8_t InfoQueryBuilder::environmentToByte(InfoQueryBuilder::ENVIRONMENT environment) const { ;
    switch(environment) {
        case ENVIRONMENT::LINUX:
            return isObsolete() ? 'L' : 'l';
        case ENVIRONMENT::WINDOWS:
            return isObsolete() ? 'W' : 'w';
        case ENVIRONMENT::MAC:
            return isObsolete() ? environmentToByte(ENVIRONMENT::LINUX) : 'o';
    }
    return {};
}

InfoQueryBuilder::ENVIRONMENT InfoQueryBuilder::byteToEnvironment(uint8_t byte) const {
    bool obsolete = isObsolete();

    switch(std::tolower(byte)) {
        case 'l':
            return ENVIRONMENT::LINUX;
        case 'w':
            return ENVIRONMENT::WINDOWS;
        case 'o':
        case 'm':
            if(!obsolete) {
                return ENVIRONMENT::MAC;
            } else {
                throw InvalidPacketException();
            }
        default: {
            throw InvalidPacketException();
        }
    }
}

bool InfoQueryBuilder::isObsolete() const {
    bool obsolete = header_ == HEADER_FORMAT::OBSOLETE;
    return obsolete;
}

InfoQueryBuilder &InfoQueryBuilder::loadFromRawByteData(const std::vector<uint8_t> &v) {
    RawByteReader rr(v);

    rr.read_primitive_two_options(header_, {'m', HEADER_FORMAT::OBSOLETE}, {'I', HEADER_FORMAT::SOURCE});

    if(isObsolete()) rr.read_string(address_);
    if(!isObsolete()) rr.read_primitive(protocol_);

    rr.read_string(name_);
    rr.read_string(map_);
    rr.read_string(folder_);
    rr.read_string(game_);
    if(!isObsolete()) rr.read_primitive(id_);
    rr.read_primitive(players_);
    rr.read_primitive(maxPlayers_);
    if(!isObsolete()) rr.read_primitive(bots_);
    if(isObsolete()) rr.read_primitive(protocol_);

    rr.read_primitive_with_predicate<SERVER_TYPE>(serverType_, std::bind(&InfoQueryBuilder::byteToServerType, this, std::placeholders::_1));
    rr.read_primitive_with_predicate<ENVIRONMENT>(environment_, std::bind(&InfoQueryBuilder::byteToEnvironment, this, std::placeholders::_1));
    rr.read_primitive_two_options(visibility_, {0, VISIBILITY::PUBLIC}, {1, VISIBILITY::PRIVATE});

    if(isObsolete()) {
        modInfoObsolete_ = ModInfoObsoleteBuilder()
            .loadFromRawData(v, rr.getCurPos(), &rr.getCurPos())
            .build();
    }
    rr.read_primitive_two_options<VAC>(vac_, {1, VAC::SECURED}, {0, VAC::UNSECURED});
    if(isObsolete()) rr.read_primitive(bots_);
    if(!isObsolete()) rr.read_string(version_);
    if(!isObsolete()) rr.read_data_until_end(additionalData_);

    return *this;
}

InfoQueryBuilder &InfoQueryBuilder::setHeader(InfoQueryBuilder::HEADER_FORMAT header) {
    header_ = header;
    return *this;
}

InfoQueryBuilder &InfoQueryBuilder::setProtocol(std::uint8_t protocol) {
    protocol_ = protocol;
    return *this;
}

InfoQueryBuilder &InfoQueryBuilder::setName(const std::string &name) {
    name_ = name;
    return *this;
}

InfoQueryBuilder &InfoQueryBuilder::setMap(const std::string &map) {
    map_ = map;
    return *this;
}

InfoQueryBuilder &InfoQueryBuilder::setFolder(const std::string &folder) {
    folder_ = folder;
    return *this;
}

InfoQueryBuilder &InfoQueryBuilder::setGame(const std::string &game) {
    game_ = game;
    return *this;
}

InfoQueryBuilder &InfoQueryBuilder::setId(int16_t id) {
    id_ = id;
    return *this;
}

InfoQueryBuilder &InfoQueryBuilder::setPlayers(uint8_t players) {
    players_ = players;
    return *this;
}

InfoQueryBuilder &InfoQueryBuilder::setBots(uint8_t bots) {
    bots_ = bots;
    return *this;
}

InfoQueryBuilder &InfoQueryBuilder::setServerType(InfoQueryBuilder::SERVER_TYPE serverType) {
    serverType_ = serverType;
    return *this;
}

InfoQueryBuilder &InfoQueryBuilder::setEnvironment(InfoQueryBuilder::ENVIRONMENT environment) {
    environment_ = environment;
    return *this;
}

InfoQueryBuilder &InfoQueryBuilder::setVisibility(InfoQueryBuilder::VISIBILITY visibility) {
    visibility_ = visibility;
    return *this;
}

InfoQueryBuilder &InfoQueryBuilder::setVac(InfoQueryBuilder::VAC vac) {
    vac_ = vac;
    return *this;
}

InfoQueryBuilder &InfoQueryBuilder::setModInfoObsolete(const ModInfoObsolete &modInfoObsolete) {
    modInfoObsolete_ = modInfoObsolete;
    return *this;
}

InfoQueryBuilder::HEADER_FORMAT InfoQueryBuilder::getHeader() const {
    return header_;
}

const std::string &InfoQueryBuilder::getAddress() const {
    return address_;
}

uint8_t InfoQueryBuilder::getAProtocol() const {
    return protocol_;
}

const std::string &InfoQueryBuilder::getName() const {
    return name_;
}

const std::string &InfoQueryBuilder::getMap() const {
    return map_;
}

const std::string &InfoQueryBuilder::getFolder() const {
    return folder_;
}

const std::string &InfoQueryBuilder::getGame() const {
    return game_;
}

int16_t InfoQueryBuilder::getId() const {
    return id_;
}

uint8_t InfoQueryBuilder::getPlayers() const {
    return players_;
}

uint8_t InfoQueryBuilder::getMaxPlayers() const {
    return maxPlayers_;
}

uint8_t InfoQueryBuilder::getBots() const {
    return bots_;
}

InfoQueryBuilder::SERVER_TYPE InfoQueryBuilder::getServerType() const {
    return serverType_;
}

InfoQueryBuilder::ENVIRONMENT InfoQueryBuilder::getEnvironment() const {
    return environment_;
}

InfoQueryBuilder::VISIBILITY InfoQueryBuilder::getVisibility() const {
    return visibility_;
}

InfoQueryBuilder::VAC InfoQueryBuilder::getVac() const {
    return vac_;
}

const ModInfoObsolete &InfoQueryBuilder::getModInfoObsolete() const {
    return modInfoObsolete_;
}

const std::string &InfoQueryBuilder::getVersion() const {
    return version_;
}

