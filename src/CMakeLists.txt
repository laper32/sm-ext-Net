add_library(net SHARED
    ${CMAKE_CURRENT_LIST_DIR}/extension.cpp
    ${CMAKE_CURRENT_LIST_DIR}/common/url.cpp
    ${CMAKE_CURRENT_LIST_DIR}/common/io_context.cpp
    ${CMAKE_CURRENT_LIST_DIR}/detail/ext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/http/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/http/sm_handler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/http/native.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/websocket/ws.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/websocket/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/websocket/native.cpp
    ${CMAKE_CURRENT_LIST_DIR}/net/websocket/sm_forward.cpp
)

target_include_directories(net PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/common
    ${CMAKE_CURRENT_LIST_DIR}/net
    ${CMAKE_CURRENT_LIST_DIR}/net/http
)

if (MSVC)
    add_compile_options(/utf-8)
    #set_property(TARGET net PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
    #target_link_libraries(net PRIVATE MSVCRT.lib)
endif()

set(Boost_USE_STATIC_LIBS ON)
set(OPENSSL_USE_STATIC_LIBS ON)
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED coroutine)
target_link_libraries(net PRIVATE Boost::coroutine OpenSSL::SSL fmt::fmt-header-only)

target_link_libraries(net PUBLIC amsdk)
add_definitions(-DASIO_HAS_STD_COROUTINE -DASIO_HAS_CO_AWAIT)

if (MSVC)
    target_compile_options(net PRIVATE /bigobj)
endif()

set_target_properties(net PROPERTIES PREFIX "" SUFFIX ".ext${LIB_SHARED_EXT}")

