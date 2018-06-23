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

#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>

#include "ModInfoObsolete.h"
#include "RawByteBuilder.h"

#include <stdexcept>
#include <exception>

class InvalidPacketException : public std::runtime_error {
public:
    InvalidPacketException() : std::runtime_error("Invalid packet") {}
};

class InfoQueryBuilder {
public:
    explicit InfoQueryBuilder(const std::vector<uint8_t> &v);
    InfoQueryBuilder() = default;

    enum class HEADER_FORMAT {
        SOURCE,
        OBSOLETE
    };

    enum class SERVER_TYPE {
        DEDICATED,
        NON_DEDICATED,
        TV,
    };

    enum class ENVIRONMENT  {
        LINUX,
        WINDOWS,
        MAC,
    };

    enum class VISIBILITY {
        PUBLIC,
        PRIVATE,
    };

    enum class VAC {
        UNSECURED,
        SECURED,
    };

    InfoQueryBuilder &setHeader(HEADER_FORMAT header);
    InfoQueryBuilder &setProtocol(std::uint8_t protocol);
    InfoQueryBuilder &setName(const std::string &name);
    InfoQueryBuilder &setMap(const std::string &map);
    InfoQueryBuilder &setFolder(const std::string &folder);
    InfoQueryBuilder &setGame(const std::string &game);
    InfoQueryBuilder &setId(int16_t id);
    InfoQueryBuilder &setPlayers(uint8_t players);
    InfoQueryBuilder &setBots(uint8_t bots);
    InfoQueryBuilder &setServerType(SERVER_TYPE serverType);
    InfoQueryBuilder &setEnvironment(ENVIRONMENT environment);
    InfoQueryBuilder &setVisibility(VISIBILITY visibility);
    InfoQueryBuilder &setVac(VAC vac);
    InfoQueryBuilder &setModInfoObsolete(const ModInfoObsolete &modInfoObsolete);

    HEADER_FORMAT getHeader() const;
    const std::string &getAddress() const;
    uint8_t getAProtocol() const;
    const std::string &getName() const;
    const std::string &getMap() const;
    const std::string &getFolder() const;
    const std::string &getGame() const;
    int16_t getId() const;
    uint8_t getPlayers() const;
    uint8_t getMaxPlayers() const;
    uint8_t getBots() const;
    SERVER_TYPE getServerType() const;
    ENVIRONMENT getEnvironment() const;
    VISIBILITY getVisibility() const;
    VAC getVac() const;
    const ModInfoObsolete &getModInfoObsolete() const;
    const std::string &getVersion() const;

    InfoQueryBuilder &loadFromRawByteData(const std::vector<uint8_t> &v);
    std::vector<uint8_t> buildRawByteData();
private:

    HEADER_FORMAT header_ = {};

    std::string address_; // Obsolete

    std::uint8_t protocol_ = {};
    std::string name_;
    std::string map_;
    std::string folder_;
    std::string game_;
    std::int16_t id_ = {};
    std::uint8_t players_ = {};
    std::uint8_t maxPlayers_ = {};

    std::uint8_t bots_ = {};
    SERVER_TYPE serverType_ = {};
    ENVIRONMENT environment_ = {};
    VISIBILITY visibility_ = {};

    VAC vac_ = {};
    ModInfoObsolete modInfoObsolete_;
    std::string version_ = {};
    std::vector<uint8_t> additionalData_ = {};

    uint8_t serverTypeToByte(SERVER_TYPE serverType) const;
    SERVER_TYPE byteToServerType(uint8_t byte) const;

    uint8_t environmentToByte(ENVIRONMENT environment) const;
    ENVIRONMENT byteToEnvironment(uint8_t byte) const;

    bool isObsolete() const;
};