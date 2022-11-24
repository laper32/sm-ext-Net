#ifndef _NET_MAIN_H
#define _NET_MAIN_H

#pragma once

#include <net/shared.h>

namespace net {

bool SDK_OnLoad(char* error, size_t maxlen, bool late);
void SDK_OnUnload();

}

#endif // !_NET_MAIN_H
