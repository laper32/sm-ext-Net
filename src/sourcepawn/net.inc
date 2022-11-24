#if defined _net_included
#endinput
#endif

#define _net_included

#include <net/http>
#include <net/websocket>

public Extension __ext_net =
{
	name = "Net",
	file = "net.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};
