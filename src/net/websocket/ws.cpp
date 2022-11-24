#include <net/websocket/ws.h>

namespace net::websocket {

Url url = {};
bool use_ssl = {};
asio2::ws_client cl;
asio2::wss_client cl_ssl;

} // namespace net::websocket