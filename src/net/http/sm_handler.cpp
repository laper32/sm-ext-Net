#include <net/http/sm_handler.h>
#include <net/http/message.h>

namespace net::http::hndl {

HandleType_t hRequest = 0;
RequestHandler sRequestHandler;

HandleType_t hResponse = 0;
ResponseHandler sResponseHandler;

} // namespace net::http::hndl

void net::http::RequestHandler::OnHandleDestroy(HandleType_t type, void* object)
{
	delete (Request*)(object);
}

void net::http::ResponseHandler::OnHandleDestroy(HandleType_t type, void* object)
{
	delete (Response*)(object);
}