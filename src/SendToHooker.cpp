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

#include "helper/external_api.h"
#include "InfoQueryBuilder.h"
#include "PlayerQueryBuilder.h"
#include "SendToHooker.h"

#include <functional>
#include <iostream>

#include <sys/socket.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wextra"
#include <chooker.h>
#pragma GCC diagnostic pop
#include <api/amxx_support.h>

typedef size_t ( *SendToPtr )( int, const void*, size_t, int, const struct sockaddr*, socklen_t );

std::unique_ptr<CHooker> sendto_hooker;
SendToPtr sendto_ptr = nullptr;
CFunc* sendto_hook = nullptr;

size_t sendto_handler( int socket, const void* message, size_t length, int flags, const struct sockaddr* dest_addr, socklen_t dest_len ) {
    const unsigned char *origMessage = (unsigned char *) message;

    auto send_data = [&](const void *new_message, size_t new_length) {
        size_t ret = 0;
        if (sendto_hook->Restore()) {
            ret = sendto_ptr(socket, new_message, new_length, flags, dest_addr, dest_len);
            sendto_hook->Patch();
        }
        return ret;
    };

    auto send_new_message = [&](std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> functor) {
        std::vector<uint8_t> d;
        d.assign(origMessage + 4, origMessage + length);
        auto rawData = functor(d);
        rawData.insert(rawData.begin(), origMessage, origMessage + 4);
        return send_data(rawData.data(), rawData.size());
    };

    if (length > 5 && origMessage[0] == 0xff && origMessage[1] == 0xff && origMessage[2] == 0xff && origMessage[3] == 0xff) {
        switch (origMessage[4]) {
            case 'I':
            case 'm':
                return send_new_message([](auto d) {
                    InfoQueryBuilder qb(d);
                    int newPlayers = qb.getPlayers() + g_APIData->getPlayersDiff() + g_APIData->getPlayersNum();
                    newPlayers = std::clamp(newPlayers, 0, 255);

                    return qb
                            //.setMap("de_test2")
                            .setPlayers(newPlayers)
                            .buildRawByteData();
                });
            case 'D':
                return send_new_message([](auto d) {
                    static float lastTime = 0.0;
                    if(lastTime == 0.0) {
                        lastTime = gpGlobals->time;
                    }
                    float currentDelta = std::max(gpGlobals->time - lastTime, 0.0f);
                    lastTime = gpGlobals->time;

                    return PlayerQueryBuilder(d)
                            .push(g_APIData->getPlayers())
                            .incrementPlayersDeltaDuration(currentDelta, PlayerQueryBuilder::ONLY_CUSTOM_PLAYERS::YES)
                            .buildRawByteData();
                });
        }
    }
    return send_data(message, length);
}

std::unique_ptr<SendToHooker> SendToHooker::self_ = {};
SendToHooker::SendToHooker() {
    auto sym_ptr = ( void* )&sendto;
    while( *( unsigned short* )sym_ptr == 0x25ff )
    {
        sym_ptr = **( void*** )( ( char* )sym_ptr + 2 );
    }
    sendto_ptr = ( SendToPtr )sym_ptr;

    sendto_hooker = std::make_unique<CHooker>();
    sendto_hook = sendto_hooker->CreateHook( sendto_ptr, ( void* )sendto_handler, TRUE );
    if(sendto_hook == nullptr) {
        throw std::runtime_error("Unable to hook sendto");
    }
}

SendToHooker::~SendToHooker() {
    if(sendto_hook) {
        sendto_hook->Restore();
        sendto_hook = nullptr;
    }
    if(sendto_hooker) {
        sendto_hooker.reset();
    }
}

void SendToHooker::initInstance() {
    self_.reset(new SendToHooker);
}

void SendToHooker::freeInstance() {
    self_.reset();
}
