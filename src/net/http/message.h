#ifndef _NET_HTTP_MESSAGE_H
#define _NET_HTTP_MESSAGE_H

#pragma once

#include <net/http/shared.h>

namespace net::http {

struct Request {
	Url url;
	int timeout;
	boost::beast::http::request<boost::beast::http::string_body> data;
};
using Response = boost::beast::http::response<boost::beast::http::string_body>;

}

#endif // !_NET_HTTP_MESSAGE_H
