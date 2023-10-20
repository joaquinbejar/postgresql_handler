//
// Created by Joaquin Bejar Garcia on 20/10/23.
//

//#include <simple_logger/logger.h>
//#include <simple_config/config.h>
#include "postgresql/config.h"
#include <catch2/catch_test_macros.hpp>
//#include <common/common.h>
//#include <simple_color/color.h>


// ---------------------------------------------------------------------------------------------------
TEST_CASE("Declare PostgresqlConfig", "[PostgresqlConfig]") {
    unsetenv("MARIADB_HOSTNAME");
    unsetenv("MARIADB_PORT");
    unsetenv("MARIADB_DATABASE");
    postgresql::config::PostgresqlConfig config;
    REQUIRE(config.uri == "postgresql://user:password@localhost:5432/database");
}

