#ifndef _NET_WEBSOCKET_WS_H
#define _NET_WEBSOCKET_WS_H

#pragma once

#include <net/websocket/shared.h>

namespace net::websocket {

extern Url url;
extern bool use_ssl;
extern asio2::ws_client cl;
extern asio2::wss_client cl_ssl;


} // namespace net::websocket

#endif // !_NET_WEBSOCKET_WS_H
