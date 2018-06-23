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

#include "ModInfoObsolete.h"
#include "RawByteBuilder.h"

std::vector<uint8_t> ModInfoObsolete::as_vector() const {
    RawByteBuilder rb;
    rb.push<uint8_t>(static_cast<const uint8_t &>(mod_ == MOD::HL_MOD ? 1 : 0));
    if (mod_ == MOD::HL) {
        return std::move(rb.build());
    }

    rb.push(link_);
    rb.push(downloadLink_);
    rb.push('\0');
    rb.push(version_);
    rb.push(size_);
    rb.push<uint8_t>(static_cast<const uint8_t &>(type_ == TYPE::MULTIPLAYER_ONLY ? 1 : 0));
    rb.push<uint8_t>(static_cast<const uint8_t &>(dll_ == DLL::HL_DLL ? 0 : 1));
    return std::move(rb.build());
}