#ifndef _NET_HTTP_NATIVE_H
#define _NET_HTTP_NATIVE_H

#pragma once

#include <net/shared.h>

namespace net::http {

extern sp_nativeinfo_t g_HTTPNative[];

namespace api {

extern cell_t CreateRequest(IPluginContext*, const cell_t*);
extern cell_t SetURL(IPluginContext*, const cell_t*);
extern cell_t SetMethod(IPluginContext*, const cell_t*);
extern cell_t SetHeader(IPluginContext*, const cell_t*);
extern cell_t SetTimeout(IPluginContext*, const cell_t*);
extern cell_t SetVersion(IPluginContext*, const cell_t*);
extern cell_t SetRequestBodyText(IPluginContext*, const cell_t*);
extern cell_t SetRequestBodyJSON(IPluginContext*, const cell_t*);
extern cell_t AppendFileToRequestBody(IPluginContext*, const cell_t*);
extern cell_t PrepareRequest(IPluginContext*, const cell_t*);
extern cell_t SendRequest(IPluginContext*, const cell_t*);
extern cell_t SendRequestAsync(IPluginContext*, const cell_t*);

extern cell_t GetStatusCode(IPluginContext*, const cell_t*);
extern cell_t GetResponseLength(IPluginContext*, const cell_t*);
extern cell_t GetResponseBodyText(IPluginContext*, const cell_t*);
extern cell_t GetResponseBodyJSON(IPluginContext*, const cell_t*);
extern cell_t GetResponseBodyFile(IPluginContext*, const cell_t*);

} // namespace api

} // namespace net::http

#endif // !_NET_HTTP_NATIVE_H
