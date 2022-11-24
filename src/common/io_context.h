#ifndef _COMMON_IO_CONTEXT_H
#define _COMMON_IO_CONTEXT_H

#pragma once

#include <common/shared.h>

extern boost::asio::io_context ext_ctx;
extern boost::asio::executor_work_guard<boost::asio::io_context::executor_type> ext_guard;

#endif // !_COMMON_IO_CONTEXT_H
