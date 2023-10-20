include(FetchContent)
FetchContent_Declare(common
        GIT_REPOSITORY https://github.com/joaquinbejar/common_cpp.git
        GIT_TAG dev
)
FetchContent_MakeAvailable(common)
message(STATUS "COMMON_SOURCE_DIR ${common_SOURCE_DIR}")
set(COMMON_INCLUDE ${common_SOURCE_DIR}/include CACHE INTERNAL "")