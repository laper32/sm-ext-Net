# Build architecture. (x86 | x64)
set(TARGET_ARCH "" CACHE INTERNAL "Which arch we want to build for.")

# Path to HL2SDK.
# Noting that if this is not specified, it will be located to environment variable.
# If: Your platform is Windows, the environment variable will be HL2SDK-(Specific Game)
# Otherwise, it will be HL2SDK_(Specific Game)
# Eg: Our game is CSGO, then:
#   Windows: HL2SDK-CSGO
#   Linux: HL2SDK_CSGO
set(HL2SDK_PATH "" CACHE INTERNAL "Path to determine HL2SDK.")

# This is used for determine which game you want to build.
# Please check HL2SDK_PATH for further detail.
set(HL2SDK_GAME "" CACHE INTERNAL "Which game of SDK you want to build.")

set(ENABLE_DEBUG OFF CACHE INTERNAL "Enable debugging symbols")
set(ENABLE_OPTIMIZE ON CACHE INTERNAL "Enable optimization")

# If nothing specified, then we will obtain environment variable: METAMOD_SOURCE
set(METAMOD_PATH "" CACHE INTERNAL "Path to metamod")

# If nothing specified, then we will obtain environment variable: SOURCEMOD
set(SOURCEMOD_PATH "" CACHE INTERNAL "Path to sourcemod")