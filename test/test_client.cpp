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

//
//TEST_CASE("PostgresManager m_insert and select", "[database]") {
//    setenv("LOGLEVEL", "debug", 1);
//    postgresql::config::PostgresqlConfig config;
//    config.logger->send<simple_logger::LogLevel::INFORMATIONAL>(config.uri);
//    PostgresManager dbManager(config);
//
//    SECTION("Insert data") {
//        REQUIRE(dbManager.m_insert("INSERT INTO table_name (column1, column2) VALUES ('value1', 'value2');") == true);
//    }
//
//    SECTION("Select data") {
//        auto result = dbManager.select("SELECT * FROM table_name;");
//        REQUIRE(!result.empty());  // Assume that the table is not empty
//        REQUIRE(result[0]["column1"] == "value1");
//        REQUIRE(result[0]["column2"] == "value2");
//    }
//
//    SECTION("Insert multiple data") {
//        std::vector<std::string> queries = {
//                "INSERT INTO table_name (column1, column2) VALUES ('value1', 'value2');",
//                "INSERT INTO table_name (column1, column2) VALUES ('value3', 'value4');",
//                "INSERT INTO table_name (column1, column2) VALUES ('value5', 'value6');",
//                "INSERT INTO table_name (column1, column2) VALUES ('value7', 'value8');",
//                "INSERT INTO table_name (column1, column2) VALUES ('value9', 'value10');",
//        };
//        REQUIRE(dbManager.insert_multi(queries) == true);
//    }
//
//    SECTION("Insert multiple data one fail") {
//        std::vector<std::string> queries = {
//                "INSERT INTO table_name (column1, column2) VALUES ('fail1', 'fail1');",
//                "INSERT INTO table_name (column1, column2) VALUES ('fail2', 'fail2');",
//                "INSERT INTO table_name (column1, column2) VALUES (fail, 'fail3');",
//                "INSERT INTO table_name (column1, column2) VALUES ('fail4', 'fail4');",
//                "INSERT INTO table_name (column1, column2) VALUES ('fail5', 'fail5');",
//        };
//        REQUIRE(dbManager.insert_multi(queries) == false);
//
//    }
//
//}
//
//bool localinsert(PostgresManager &dbManager, const std::vector<std::string> &queries) {
//    std::vector<bool> results;
//    results.reserve(queries.size());
//    for (auto &query: queries) {
//        results.push_back(dbManager.m_insert(query));
//    }
//    return std::all_of(results.begin(), results.end(), [](bool i) { return i; });
//}
//
//TEST_CASE("PostgresManager insert_multi performance", "[.benchmark]") {
//    setenv("LOGLEVEL", "debug", 1);
//    postgresql::config::PostgresqlConfig config;
//    config.logger->send<simple_logger::LogLevel::INFORMATIONAL>(config.uri);
//    PostgresManager dbManager(config);
//
//    std::vector<std::string> queries;
//    queries.reserve(100);
//    for (int i = 0; i < 100; ++i) {
//        queries.push_back("INSERT INTO table_name (column1, column2) VALUES ('value" + std::to_string(i) + "', 'value" +
//                          std::to_string(i) + "');");
//    }
//
//
//    BENCHMARK("Insert 10 rows insert_multi") {
//                                                 return dbManager.insert_multi(queries);
//                                             };
////    BENCHMARK("Insert 10 rows m_insert") {
////                                            return localinsert(dbManager, queries);
////                                        };
//
//}
//
//
//TEST_CASE("Testing isInsertOrReplaceQueryCorrect function", "[sql]") {
//    REQUIRE(is_insert_or_replace_query_correct(
//            "INSERT INTO table_name (column1, column2) VALUES ('value1', 'value2');") == true);
//    REQUIRE(is_insert_or_replace_query_correct("REPLACE INTO table_name (column1, column2) VALUES (value1, value2);") ==
//            true);
//    REQUIRE(is_insert_or_replace_query_correct("INSERTINTO table_name (column1, column2) VALUES (value1, value2);") ==
//            false);
//    REQUIRE(is_insert_or_replace_query_correct("INSERT INTO table_name column1, column2 VALUES (value1, value2);") ==
//            false);
//    REQUIRE(is_insert_or_replace_query_correct("INSERT INTO table_name (column1, column2) VALUES (value1, value2") ==
//            false);
//}
//
//TEST_CASE("Testing PostgresManager::enqueue method", "[postgres]") {
//    setenv("LOGLEVEL", "debug", 1);
//    postgresql::config::PostgresqlConfig config;
//    config.logger->send<simple_logger::LogLevel::INFORMATIONAL>(config.uri);
//    PostgresManager dbManager(config);
//
//    SECTION("Handling correct queries") {
//        REQUIRE(dbManager.enqueue("INSERT INTO table_name (column1, column2) VALUES (value1, value2);") == true);
//        REQUIRE(dbManager.enqueue("REPLACE INTO table_name (column1, column2) VALUES (value1, value2);") == true);
//    }
//
//    SECTION("Handling incorrect queries") {
//        REQUIRE(dbManager.enqueue("INSERTINTO table_name (column1, column2) VALUES (value1, value2);") == false);
//        REQUIRE(dbManager.enqueue("INSERT INTO table_name column1, column2 VALUES (value1, value2);") == false);
//        REQUIRE(dbManager.enqueue("INSERT INTO table_name (column1, column2) VALUES (value1, value2") == false);
//    }
//}

TEST_CASE("Testing ThreadQueue functionality", "[queue]") {

    setenv("LOGLEVEL", "debug", 1);
    postgresql::config::PostgresqlConfig config;
    config.logger->send<simple_logger::LogLevel::INFORMATIONAL>(config.to_string());
    PostgresManager dbManager(config);
//    dbManager.stop(); // no thread

    SECTION("Select data") {
        REQUIRE(dbManager.enqueue("INSERT INTO table_name (column1, column2) VALUES ('value1', 'value2');") == true);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        auto result = dbManager.select("SELECT * FROM table_name where column1 = 'value1' and column2 = 'value2';");
        REQUIRE(!result.empty());  // Assume that the table is not empty
        REQUIRE(result[0]["column1"] == "value1");
        REQUIRE(result[0]["column2"] == "value2");
    }


    SECTION("Testing enqueue method") {
        REQUIRE(dbManager.enqueue("INSERT INTO table_name (column1, column2) VALUES ('value3', 'value4');") == true);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }

    SECTION("Testing enqueue multi queries method") {
        for (int i = 0; i < 100; ++i) {
            REQUIRE(dbManager.enqueue("INSERT INTO table_name (column1, column2) VALUES ('value_for', 'value" + std::to_string(i) + "');") == true);
        }
        std::this_thread::sleep_for(std::chrono::seconds(10));
        auto result = dbManager.select("SELECT * FROM table_name where column1 = 'value_for';");
        REQUIRE(!result.empty());  // Assume that the table is not empty
        REQUIRE(result.size() == 100);
    }

    //    SECTION("Insert multiple data") {
//        std::vector<std::string> queries = {
//                "INSERT INTO table_name (column1, column2) VALUES ('value1', 'value2');",
//                "INSERT INTO table_name (column1, column2) VALUES ('value3', 'value4');",
//                "INSERT INTO table_name (column1, column2) VALUES ('value5', 'value6');",
//                "INSERT INTO table_name (column1, column2) VALUES ('value7', 'value8');",
//                "INSERT INTO table_name (column1, column2) VALUES ('value9', 'value10');",
//        };
//        REQUIRE(dbManager.insert_multi(queries) == true);
//    }


}
