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

#include "ModInfoObsoleteBuilder.h"
#include "ModInfoObsolete.h"
#include "RawByteReader.h"

ModInfoObsoleteBuilder &ModInfoObsoleteBuilder::setLink(const std::string &link) {
    modInfoObsolete.link_ = link;
    return *this;
}

ModInfoObsoleteBuilder &ModInfoObsoleteBuilder::setDownloadLink(const std::string &downloadLink) {
    modInfoObsolete.downloadLink_ = downloadLink;
    return *this;
}

ModInfoObsoleteBuilder &ModInfoObsoleteBuilder::setVersion(std::int32_t version) {
    modInfoObsolete.version_ = version;
    return *this;
}

ModInfoObsoleteBuilder &ModInfoObsoleteBuilder::setSize(std::size_t size) {
    modInfoObsolete.size_ = size;
    return *this;
}

ModInfoObsoleteBuilder &ModInfoObsoleteBuilder::setType(ModInfoObsolete::TYPE type) {
    modInfoObsolete.type_ = type;
    return *this;
}

ModInfoObsoleteBuilder &ModInfoObsoleteBuilder::setDll(ModInfoObsolete::DLL dll) {
    modInfoObsolete.dll_ = dll;
    return *this;
}

ModInfoObsolete ModInfoObsoleteBuilder::build(ModInfoObsolete::MOD mod) {
    modInfoObsolete.mod_ = mod;
    return build();
}

ModInfoObsolete ModInfoObsoleteBuilder::build() {
    return modInfoObsolete;
}

ModInfoObsoleteBuilder &ModInfoObsoleteBuilder::loadFromRawData(const std::vector<uint8_t> &in, std::size_t *newCurPos) {
    RawByteReader rr(in);
    rr.read_primitive_two_options(modInfoObsolete.mod_, {1, ModInfoObsolete::MOD::HL_MOD}, {0, ModInfoObsolete::MOD::HL});
    if(modInfoObsolete.mod_ == ModInfoObsolete::MOD::HL) {
        return *this;
    }

    rr.read_string(modInfoObsolete.link_);
    rr.read_string(modInfoObsolete.downloadLink_);
    rr.skip_byte();
    rr.read_primitive(modInfoObsolete.version_);
    rr.read_primitive(modInfoObsolete.size_);
    rr.read_primitive_two_options(modInfoObsolete.type_, {1, ModInfoObsolete::TYPE::MULTIPLAYER_ONLY}, {0, ModInfoObsolete::TYPE::SINGLE_AND_MULTIPLAYER});
    rr.read_primitive_two_options(modInfoObsolete.dll_, {0, ModInfoObsolete::DLL::HL_DLL}, {1, ModInfoObsolete::DLL::OWN_DLL});
    if(newCurPos) *newCurPos += rr.getCurPos();
    return *this;
}

ModInfoObsoleteBuilder &ModInfoObsoleteBuilder::loadFromRawData(const std::vector<uint8_t> &in, std::size_t offset, std::size_t *newCurPos) {
    std::vector<uint8_t> data;
    data.reserve(in.size() - offset);
    std::copy_n(in.begin() + offset, in.size() - offset, std::back_inserter(data));
    return loadFromRawData(data, newCurPos);
}