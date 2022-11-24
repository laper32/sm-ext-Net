if (METAMOD_PATH STREQUAL "")
	set(METAMOD_PATH $ENV{METAMOD_SOURCE})
endif()

string(REPLACE "\\" "/" METAMOD_PATH ${METAMOD_PATH})

message(${METAMOD_PATH})

add_library(mmsdk INTERFACE)
target_include_directories(mmsdk INTERFACE
	${METAMOD_PATH}/core
	${METAMOD_PATH}/public
	${METAMOD_PATH}/public/sourcehook
)