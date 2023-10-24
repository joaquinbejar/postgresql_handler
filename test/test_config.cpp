//
// Created by Joaquin Bejar Garcia on 20/10/23.
//


#include "postgresql/config.h"
#include <catch2/catch_test_macros.hpp>



// ---------------------------------------------------------------------------------------------------
TEST_CASE("Declare PostgresqlConfig", "[PostgresqlConfig]") {
    unsetenv("PG_HOSTNAME");
    unsetenv("PG_PORT");
    unsetenv("PG_DATABASE");
    postgresql::config::PostgresqlConfig config;
    REQUIRE(config.uri == "postgresql://:@:5432/");
    REQUIRE_FALSE(config.validate());
}

TEST_CASE("Declare PostgresqlConfig with env variables", "[PostgresqlConfig]") {
    setenv("PG_HOSTNAME", "localhost", 1);
    setenv("PG_PORT", "5432", 1);
    setenv("PG_DATABASE", "database", 1);
    postgresql::config::PostgresqlConfig config;
    REQUIRE(config.uri == "postgresql://:@localhost:5432/database");
    REQUIRE_FALSE(config.validate());
}

TEST_CASE("Declare PostgresqlConfig with env variables full valid", "[PostgresqlConfig]") {
    setenv("PG_HOSTNAME", "localhost", 1);
    setenv("PG_PORT", "5432", 1);
    setenv("PG_DATABASE", "database", 1);
    setenv("PG_USER", "user", 1);
    setenv("PG_PASSWORD", "password", 1);
    postgresql::config::PostgresqlConfig config;
    REQUIRE(config.uri == "postgresql://user:password@localhost:5432/database");
    REQUIRE(config.validate());
}

TEST_CASE("Use Logger", "[PostgresqlConfig]") {
    setenv("PG_HOSTNAME", "localhost", 1);
    setenv("PG_PORT", "5432", 1);
    setenv("PG_DATABASE", "database", 1);
    setenv("PG_USER", "user", 1);
    setenv("PG_PASSWORD", "password", 1);
    postgresql::config::PostgresqlConfig config;
    REQUIRE(config.uri == "postgresql://user:password@localhost:5432/database");
    REQUIRE(config.validate());
    std::shared_ptr<simple_logger::Logger> logger = config.logger;
    logger->send<simple_logger::LogLevel::EMERGENCY>("EMERGENCY message");
}