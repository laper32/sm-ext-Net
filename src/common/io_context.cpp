#include <common/io_context.h>

boost::asio::io_context ext_ctx;
boost::asio::executor_work_guard<boost::asio::io_context::executor_type> ext_guard = boost::asio::make_work_guard(ext_ctx);
