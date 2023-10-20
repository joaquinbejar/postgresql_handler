cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(postgresql_handler)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
set(POSTGRESQL_HANDLER_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "POSTGRESQL_HANDLER_INCLUDE ${POSTGRESQL_HANDLER_INCLUDE}")

include(cmake/common.cmake)
include(cmake/simple_config.cmake)
include(cmake/simple_logger.cmake)

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
)
target_link_libraries(postgresql_handler PUBLIC
        simple_config
        simple_logger
        common
)

install(TARGETS postgresql_handler DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

option(NO_POSTGRESQL_HANDLER_TESTS "Disable Current tests" OFF)

if (NOT NO_POSTGRESQL_HANDLER_TESTS)
    include(FetchContent)

    FetchContent_Declare(Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.4.0
    )
    FETCHCONTENT_GETPROPERTIES(Catch2)
    FETCHCONTENT_MAKEAVAILABLE(Catch2)

    add_subdirectory(test)
endif ()

