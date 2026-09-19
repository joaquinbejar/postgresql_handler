cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(postgresql_handler VERSION 0.1.0)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
set(POSTGRESQL_HANDLER_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "POSTGRESQL_HANDLER_INCLUDE ${POSTGRESQL_HANDLER_INCLUDE}")

include(FetchContent)
# Declared before the sub-dependencies so this pin wins over the Catch2 they
# declare themselves (FetchContent honours the first declaration).
FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.16.0
)
# nlohmann_json is only pulled in transitively (common_cpp v0.1.5 pins v3.11.2,
# whose cmake_minimum_required is rejected by CMake >= 4.0); pin the current
# release here so it takes precedence.
FetchContent_Declare(nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)
include(cmake/common.cmake)
include(cmake/simple_config.cmake)
include(cmake/simple_logger.cmake)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/FindLibPQ.cmake")
find_package(LibPQ REQUIRED)


add_library(postgresql_handler STATIC
        src/client.cpp
        include/postgresql/client.h
        src/config.cpp
        include/postgresql/config.h
        src/exceptions.cpp
        include/postgresql/exceptions.h
)
target_include_directories(postgresql_handler PRIVATE
        ${POSTGRESQL_HANDLER_INCLUDE}
        ${SIMPLE_CONFIG_INCLUDE}
        ${SIMPLE_LOGGER_INCLUDE}
        ${NLOHMANN_JSON_INCLUDE}
        ${COMMON_INCLUDE}
        ${LIBPQ_INCLUDE}
        ${POSTGRES_INCLUDE}
)
target_link_libraries(postgresql_handler PUBLIC
        simple_config
        simple_logger
        common
)
if(LibPQ_FOUND)
    target_link_libraries(postgresql_handler PUBLIC LibPQ::PQ)
endif()

install(TARGETS postgresql_handler DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

option(NO_POSTGRESQL_HANDLER_TESTS "Disable Current tests" OFF)

if (NOT NO_POSTGRESQL_HANDLER_TESTS)
    FETCHCONTENT_GETPROPERTIES(Catch2)
    FETCHCONTENT_MAKEAVAILABLE(Catch2)

    add_subdirectory(test)
endif ()

