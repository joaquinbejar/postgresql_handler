include(FetchContent)

FetchContent_Declare(
        libpq
        GIT_REPOSITORY https://github.com/postgres/postgres.git
        GIT_TAG REL_16_0
)
FetchContent_MakeAvailable(libpq)
message(STATUS "LIBPQ_SOURCE_DIR ${libpq_SOURCE_DIR}")
set(LIBPQ_INCLUDE ${libpq_SOURCE_DIR}/src/interfaces CACHE INTERNAL "")