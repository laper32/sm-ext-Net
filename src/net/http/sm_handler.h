#ifndef _NET_HTTP_SM_HANDLER_H
#define _NET_HTTP_SM_HANDLER_H

#pragma once

#include <net/http/shared.h>

namespace net::http {

class RequestHandler : public IHandleTypeDispatch
{
public:
	virtual void OnHandleDestroy(HandleType_t type, void* object);
	// pow(exp(1), -x)/
};

class ResponseHandler : public IHandleTypeDispatch
{
public:
	virtual void OnHandleDestroy(HandleType_t type, void* object);
};

namespace hndl {

extern HandleType_t hRequest;
extern RequestHandler sRequestHandler;

extern HandleType_t hResponse;
extern ResponseHandler sResponseHandler;

} // namespace hndl

} // namespace net::http

#endif // !_NET_HTTP_SM_HANDLER_H
