#ifndef _NET_WEBSOCKET_NATIVE_H
#define _NET_WEBSOCKET_NATIVE_H

#pragma once

#include <net/websocket/shared.h>

namespace net::websocket {

extern sp_nativeinfo_t g_WebSocketNative[];

namespace api {

extern cell_t Init(IPluginContext*, const cell_t*);
extern cell_t UseSSL(IPluginContext*, const cell_t*);
extern cell_t AddCertificates(IPluginContext*, const cell_t*);
extern cell_t SetURL(IPluginContext*, const cell_t*);
extern cell_t Connect(IPluginContext*, const cell_t*);
extern cell_t Connected(IPluginContext*, const cell_t*);
extern cell_t Disconnect(IPluginContext*, const cell_t*);
extern cell_t Send(IPluginContext*, const cell_t*);

} // namespace api

} // namespace net::websocket

#endif // !_NET_WEBSOCKET_NATIVE_H
