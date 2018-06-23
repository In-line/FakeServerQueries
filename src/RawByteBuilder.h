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

#ifndef FAKESERVERQUERIES_RAWBYTEBUILDER_H
#define FAKESERVERQUERIES_RAWBYTEBUILDER_H

#include <algorithm>
#include <vector>
#include <cstdint>
#include <type_traits>

class RawByteBuilder {
public:
    template<typename T>
    RawByteBuilder &push(const T &v, std::size_t size) {
        auto bytes = reinterpret_cast<const uint8_t*>(&v);
        std::copy_n(bytes, size, std::back_inserter(vector_));
        return *this;
    }

    template<typename T>
    inline RawByteBuilder &push(const T *v, std::size_t size) {
        return push(*v, size);
    }

    template<typename T>
    inline RawByteBuilder &push(const T &v) {
        return push(v, sizeof(v));
    }

    std::vector<uint8_t> build();
private:
    std::vector<uint8_t> vector_;
};

template<>
RawByteBuilder &RawByteBuilder::push(const std::string_view &v);

template<>
RawByteBuilder &RawByteBuilder::push(char *const &v);

template<>
RawByteBuilder &RawByteBuilder::push(const std::string &v);

template<>
RawByteBuilder &RawByteBuilder::push(const std::vector<std::uint8_t> &v);

#endif //FAKESERVERQUERIES_RAWBYTEBUILDER_H
