#ifndef _NET_SHARED_H
#define _NET_SHARED_H

#pragma once

#include <extension.h>
#include <common/url.hpp>
#include <common/shared.h>
#include <common/io_context.h>
#include <extensions/ILog4SM.h>
#include <extensions/IJSON.h>

namespace net {

extern ILog4SM* log4sm;
extern IJSON* json;
namespace logger {

extern ILogger* net;
extern ILogger* net_http;
extern ILogger* net_websocket;

} // namespace logger

} // namespace net

#endif // !_NET_SHARED_H
