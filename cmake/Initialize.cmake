if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(LIB_STATIC_EXT ".a")
    set(LIB_STATIC_EXT ".dylib")
    set(LIB_PREFIX "lib")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LIB_STATIC_EXT ".a")
    set(LIB_SHARED_EXT ".so")
    set(LIB_PREFIX "lib")

    if (TARGET_ARCH MATCHES "x64")
        set(LIB_STATIC_SUFFIX "")
    else()
        set(LIB_STATIC_SUFFIX "_i486")
    endif()
elseif(WIN32)
    set(LIB_STATIC_EXT ".lib")
    set(LIB_SHARED_EXT ".dll")
    set(LIB_STATIC_SUFFIX "")
    # If have WIN64 then do further things
endif()

if (HL2SDK_PATH STREQUAL "")
    # Require: HL2SDK-${specific_engine_name}
    # Please follow this rule to preconfigure your SDK path.
    set(HL2SDK_ENVVAR "hl2sdk-${HL2SDK_GAME}")
    string(TOUPPER "${HL2SDK_ENVVAR}" HL2SDK_ENVVAR)

    if (NOT WIN32)
        string(REPLACE "-" "_" HL2SDK_ENVVAR "${HL2SDK_ENVVAR}")
    endif()

    if (DEFINED ENV{${HL2SDK_ENVVAR}})
        set(HL2SDK_PATH_STRING $ENV{${HL2SDK_ENVVAR}})
        # Path of windows are all '\'
        # We need to make sure that everything is under '/'
        string(REPLACE "\\" "/" HL2SDK_PATH_STRING "${HL2SDK_PATH_STRING}")
    else()
        message(FATAL_ERROR "You want to use environment variable but it does not exist. Check environment variable: ${HL2SDK_ENVVAR}")
    endif()
endif()

set(HL2SDK_PATH ${HL2SDK_PATH_STRING} CACHE PATH "Path to Half life 2 SDK" FORCE)

if (TARGET_ARCH MATCHES "x64")
    set(HL2SDK_64BIT ON)
elseif(TARGET_ARCH MATCHES "x86")
    set(HL2SDK_64BIT OFF)
else()
    message(FATAL_ERROR "Unsupported target architecture: ${TARGET_ARCH}")
endif()