#ifndef _NET_HTTP_SHARED_H
#define _NET_HTTP_SHARED_H

#pragma once

#include <net/shared.h>

namespace net::http::detail {

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>


} // namespace detail


#endif // !_NET_HTTP_SHARED_H
