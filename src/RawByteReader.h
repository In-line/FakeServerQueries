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
// Created by alik on 5/20/18.
//

#ifndef FAKESERVERQUERIES_RAWBYTEREADER_H
#define FAKESERVERQUERIES_RAWBYTEREADER_H

#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <functional>

#include <cstring>
#include <unitypes.h>


class RawByteReader {
public:
    class OutOfRangeError : public std::out_of_range {
    public:
        OutOfRangeError() : std::out_of_range("Reader is out of range") {
        }
    };

    explicit RawByteReader(const std::vector<std::uint8_t> &data);
    std::string &read_string(std::string &str);
    std::vector<uint8_t> &read_data_until_end(std::vector<uint8_t> &in);

    template<typename T>
    T read_primitive(T &in) {
        auto size = sizeof(in);
        if(curPos_ + size > data_.size()) throw OutOfRangeError();

        std::memcpy(&in, data_.data() + curPos_, size);
        curPos_ += size;
        return in;
    }

    template<typename T>
    T read_primitive_with_predicate(T &in, const std::function<T(std::uint8_t)> &predicate) {
        uint8_t tmpByte;
        in = predicate(read_primitive(tmpByte));
        return in;
    }

    template <typename T>
    T read_primitive_two_options(T &in, std::pair<uint8_t, T> a, std::pair<uint8_t, T> b) {
        return read_primitive_with_predicate<T>(in, [&](std::uint8_t byte) -> T {
            if(byte == a.first) {
                return a.second;
            } else if(byte == b.first) {
                return b.second;
            } else {
                throw OutOfRangeError();
            }
        });
    }

    void skip_byte();
    std::size_t &getCurPos();
private:
    std::vector<std::uint8_t> data_;
    std::size_t curPos_ = {};
};


#endif //FAKESERVERQUERIES_RAWBYTEREADER_H
