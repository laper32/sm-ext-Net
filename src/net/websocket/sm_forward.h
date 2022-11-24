#ifndef _NET_WEBSOCKET_SM_FORWARD_H
#define _NET_WEBSOCKET_SM_FORWARD_H

#pragma once

#include <net/websocket/shared.h>

namespace net::websocket {

namespace forward {

extern IForward* OnMessageReceived;

} // namespace forward

} // namespace sm::websocket

#endif // !_NET_WEBSOCKET_SM_FORWARD_H
