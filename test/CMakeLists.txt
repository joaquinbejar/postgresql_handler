
add_executable(test_client test_client.cpp)
target_include_directories(test_client
        PRIVATE
        ${POSTGRESQL_HANDLER_INCLUDE}
        ${SIMPLE_CONFIG_INCLUDE}
        ${SIMPLE_LOGGER_INCLUDE}
        ${NLOHMANN_JSON_INCLUDE}
        ${COMMON_INCLUDE}
        ${LIBPQ_INCLUDE}
)

target_link_libraries(test_client PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test_client PRIVATE
        simple_logger
        simple_config
        common
        postgresql_handler
        libpq
)


add_executable(test_config test_config.cpp)
target_include_directories(test_config
        PRIVATE
        ${POSTGRESQL_HANDLER_INCLUDE}
        ${SIMPLE_CONFIG_INCLUDE}
        ${SIMPLE_LOGGER_INCLUDE}
        ${NLOHMANN_JSON_INCLUDE}
        ${COMMON_INCLUDE}
)

target_link_libraries(test_config PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test_config PRIVATE
        simple_logger
        simple_config
        common
        postgresql_handler
)