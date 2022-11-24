#ifndef _NET_WEBSOCKET_MAIN_H
#define _NET_WEBSOCKET_MAIN_H

#pragma once

#include <net/websocket/shared.h>

namespace net::websocket {

bool SDK_OnLoad(char* error, size_t maxlen, bool late);
void SDK_OnUnload();

} // namespace net::websocket

#endif // !_NET_WEBSOCKET_MAIN_H
