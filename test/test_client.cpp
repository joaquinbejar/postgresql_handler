//
// Created by Joaquin Bejar Garcia on 1/2/23.
//

#include <simple_logger/logger.h>
#include <simple_config/config.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <postgresql/client.h>


// ---------------------------------------------------------------------------------------------------
using postgresql::client::PostgresManager;
using postgresql::client::is_insert_or_replace_query_correct;


TEST_CASE("Testing ThreadQueue multi-insert functionality", "[queue]") {
    setenv("PG_MULTI_INSERT", "true", 1);
    setenv("LOGLEVEL", "debug", 1);
    postgresql::config::PostgresqlConfig config;
//    config.logger->send<simple_logger::LogLevel::INFORMATIONAL>(config.to_string());
    PostgresManager dbManager(config);
    auto id = common::key_generator();
//    dbManager.stop(); // no thread

    SECTION("Select data") {
        REQUIRE(dbManager.enqueue("INSERT INTO table_name (column1, column2) VALUES ('" + id + "', 'value2');") == true);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        auto result = dbManager.select("SELECT * FROM table_name where column1 = '" + id + "' and column2 = 'value2';");
        REQUIRE(!result.empty());  // Assume that the table is not empty
        REQUIRE(result[0]["column1"] == id);
        REQUIRE(result[0]["column2"] == "value2");
    }



    SECTION("Testing enqueue multi queries method") {
        for (int i = 0; i < 100; ++i) {
            REQUIRE(dbManager.enqueue("INSERT INTO table_name (column1, column2) VALUES ('value_for_" + id + "', 'value" + std::to_string(i) + "');") == true);
        }
        REQUIRE(dbManager.queue_size() > 0);
        std::this_thread::sleep_for(std::chrono::seconds(10));
        auto result = dbManager.select("SELECT * FROM table_name where column1 = 'value_for_" + id + "';");
        REQUIRE(!result.empty());  // Assume that the table is not empty
        REQUIRE(result.size() == 100);
    }
}


TEST_CASE("Testing ThreadQueue single-insert functionality", "[queue]") {
    setenv("PG_MULTI_INSERT", "false", 1);
    setenv("LOGLEVEL", "debug", 1);
    postgresql::config::PostgresqlConfig config;
//    config.logger->send<simple_logger::LogLevel::INFORMATIONAL>(config.to_string());
    PostgresManager dbManager(config);
    auto id = common::key_generator();
//    dbManager.stop(); // no thread

    SECTION("Select data") {
        REQUIRE(dbManager.enqueue("INSERT INTO table_name (column1, column2) VALUES ('" + id + "', 'value2');") == true);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        auto result = dbManager.select("SELECT * FROM table_name where column1 = '" + id + "' and column2 = 'value2';");
        REQUIRE(!result.empty());  // Assume that the table is not empty
        REQUIRE(result[0]["column1"] == id);
        REQUIRE(result[0]["column2"] == "value2");
    }



    SECTION("Testing enqueue multi queries method") {
        for (int i = 0; i < 100; ++i) {
            REQUIRE(dbManager.enqueue("INSERT INTO table_name (column1, column2) VALUES ('value_for_" + id + "', 'value" + std::to_string(i) + "');") == true);
        }
        std::this_thread::sleep_for(std::chrono::seconds(10));
        auto result = dbManager.select("SELECT * FROM table_name where column1 = 'value_for_" + id + "';");
        REQUIRE(!result.empty());  // Assume that the table is not empty
        REQUIRE(result.size() == 100);
    }
}

TEST_CASE("Testing ThreadQueue multi-insert with fails functionality", "[queue]") {
    setenv("PG_MULTI_INSERT", "true", 1);
    setenv("LOGLEVEL", "debug", 1);
    postgresql::config::PostgresqlConfig config;
    config.logger->send<simple_logger::LogLevel::INFORMATIONAL>(config.to_string());
    PostgresManager dbManager(config);
    auto id = common::key_generator();


    SECTION("Testing enqueue multi queries method") {
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10; ++j) {
                REQUIRE(dbManager.enqueue(
                        "INSERT INTO table_name (column1, column2) VALUES ('value_for_" + id + "', 'value" +
                        std::to_string(i) + "_"+std::to_string(j) +"');") == true);
            }
            REQUIRE(dbManager.enqueue( // This query wrong syntax should fail
                    "INSERT INTO table_name (column1, column2) VALUES ('value_for_" + id + "', value" +
                    std::to_string(i) + ");") == true);
        }
        REQUIRE(dbManager.queue_size() > 0);
        std::this_thread::sleep_for(std::chrono::seconds(30));
        auto result = dbManager.select("SELECT * FROM table_name where column1 = 'value_for_" + id + "';");
        REQUIRE(!result.empty());  // Assume that the table is not empty
        REQUIRE(result.size() == 100);
    }
}