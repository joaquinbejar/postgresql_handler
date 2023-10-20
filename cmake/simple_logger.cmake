include(FetchContent)
FetchContent_Declare(simple_logger
        GIT_REPOSITORY https://github.com/joaquinbejar/simple_logger.git
        GIT_TAG dev
        )
FetchContent_MakeAvailable(simple_logger)
message(STATUS "SIMPLE_LOGGER_SOURCE_DIR ${simple_logger_SOURCE_DIR}")
set(SIMPLE_LOGGER_INCLUDE ${simple_logger_SOURCE_DIR}/include CACHE INTERNAL "")