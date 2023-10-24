//
// Created by Joaquin Bejar Garcia on 20/10/23.
//

#include "postgresql/client.h"

namespace postgresql::client {

    bool is_insert_or_replace_query_correct(const std::string &query) {

        std::regex queryRegex(
                R"(^\s*(INSERT|REPLACE)\s+INTO\s+[a-zA-Z_][a-zA-Z_0-9]*\s*\(([^)]+)\)\s*VALUES\s*\(([^)]+)\)\s*;?\s*$)",
                std::regex_constants::icase
        );

        return std::regex_match(query, queryRegex);
    }


    PostgresManager::PostgresManager(postgresql::config::PostgresqlConfig &config) : m_config(config),
                                                                                     m_queue_thread_is_running(true),
                                                                                     m_queue_thread(
                                                                                             &PostgresManager::run,
                                                                                             this) {
        conn = PQconnectdb(config.uri.c_str());
        if (PQstatus(conn) != CONNECTION_OK) {
            m_logger->send<simple_logger::LogLevel::ERROR>(
                    "Connection to database failed: " + std::string(PQerrorMessage(conn)));
            PQfinish(conn);
            exit(1);
        }
    }

    PostgresManager::~PostgresManager() {
        PQfinish(conn);
        m_queue_thread_is_running = false;
        if (m_queue_thread.joinable()) {
            m_queue_thread.join();
        }
    }

    bool PostgresManager::insert(const std::string &query) {
        std::lock_guard<std::mutex> lock(db_mutex);
        PGresult *res = PQexec(conn, query.c_str());
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            m_logger->send<simple_logger::LogLevel::ERROR>(
                    "INSERT failed: " + std::string(PQerrorMessage(conn)));
            PQclear(res);
            return false;
        }
        PQclear(res);
        return true;
    }

    bool PostgresManager::insert_multi(const std::vector<std::string> &queries) {
        std::lock_guard<std::mutex> lock(db_mutex);

        std::stringstream multi_query;
        multi_query << "BEGIN;";
        for (const auto &query: queries) {
            multi_query << query << ";";
        }
        multi_query << "COMMIT;";

        PGresult *res = PQexec(conn, multi_query.str().c_str());
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            m_logger->send<simple_logger::LogLevel::ERROR>(
                    "Multi INSERT failed: " + std::string(PQerrorMessage(conn)));
            PQclear(res);
            return false;
        }
        PQclear(res);
        return true;
    }

    std::vector<std::map<std::string, std::string>> PostgresManager::select(const std::string &query) {
        std::lock_guard<std::mutex> lock(db_mutex);
        std::vector<std::map<std::string, std::string>> result;
        PGresult *res = PQexec(conn, query.c_str());

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            m_logger->send<simple_logger::LogLevel::ERROR>(
                    "SELECT failed: " + std::string(PQerrorMessage(conn)));
            PQclear(res);
            return result;  // return empty vector
        }

        int rows = PQntuples(res);
        int cols = PQnfields(res);

        for (int i = 0; i < rows; ++i) {
            std::map<std::string, std::string> row_map;
            for (int j = 0; j < cols; ++j) {
                row_map[PQfname(res, j)] = PQgetvalue(res, i, j);
            }
            result.push_back(row_map);
        }
        PQclear(res);
        return result;
    }

    bool PostgresManager::enqueue(const std::string &query) {
        if (is_insert_or_replace_query_correct(query)) {
            return m_queries.enqueue(query);
        }
        return false;
    }


    size_t PostgresManager::queue_size() {
        return m_queries.size();
    }


    std::string PostgresManager::dequeue() {
        std::string query = "";
        if (m_queries.dequeue(query)) { // blocking
            return query;
        } else {
            return this->dequeue();
        }
    }

    void PostgresManager::stop() {
        m_queue_thread_is_running = false;
    }

    void PostgresManager::run() {
        while (m_queue_thread_is_running) {
            // sleep this thread 10 seg
//                std::this_thread::sleep_for(std::chrono::seconds(10));
            if (m_multi_insert){
                std::vector<std::string> queries;
                while (m_queries.size() > 0) {
                    queries.push_back(dequeue());
                }
                if (!queries.empty()) {
                    if (!insert_multi(queries)) { // if insert fails, try individual insert
                        for (auto &query : queries) {
                            if (!insert(query)) // if insert fails, enqueue again
                                enqueue(query);
                        }
                    }
                }
            } else {
                std::string query = dequeue();
                if (!query.empty()) {
                    if (!insert(query)) // if insert fails, enqueue again
                        enqueue(query);
                }
            }
        }
    }


}
