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
                                                                                             this)
    {
        get_connection(m_conn_select);
        if (PQstatus(m_conn_select.get()) != CONNECTION_OK) {
            m_logger->send<simple_logger::LogLevel::ERROR>(
                    "Connection to database failed: " + std::string(PQerrorMessage(m_conn_select.get())));
            PQfinish(m_conn_select.get());
            exit(1);
        }

    }

    PostgresManager::~PostgresManager() {
        this->stop();
        if (m_queue_thread.joinable()) {
            m_queue_thread.join();
        }
    }

    bool PostgresManager::m_insert(const std::string &query) {

        std::lock_guard<std::mutex> lock(m_insert_mutex);
        PGresult *res = PQexec(m_conn_insert.get(), query.c_str());

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            m_logger->send<simple_logger::LogLevel::ERROR>(
                    "INSERT failed: " + std::string(PQerrorMessage(m_conn_insert.get())));
            PQclear(res);
            return false;
        }

        PQclear(res);
        return true;
    }

    bool PostgresManager::insert_multi(const std::vector<std::string> &queries) {
        std::stringstream multi_query;

        multi_query << "BEGIN;";
        for (const auto &query: queries) {
            multi_query << query << ";";
        }
        multi_query << "COMMIT;";

        std::lock_guard<std::mutex> lock(m_insert_mutex);
        PGresult *res = PQexec(m_conn_insert.get(), multi_query.str().c_str());

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            m_logger->send<simple_logger::LogLevel::ERROR>(
                    "Multi INSERT failed: " + std::string(PQerrorMessage(m_conn_insert.get())));
            PQclear(res);
            return false;
        }
        PQclear(res);
        return true;
    }

    std::vector<std::map<std::string, std::string>> PostgresManager::select(const std::string &query) {
        std::lock_guard<std::mutex> lock(m_select_mutex);
        std::vector<std::map<std::string, std::string>> result;
        PGresult *res = PQexec(m_conn_select.get(), query.c_str());

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            m_logger->send<simple_logger::LogLevel::ERROR>(
                    "SELECT failed: " + std::string(PQerrorMessage(m_conn_select.get())));
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
        std::string query;
        if (m_queries.dequeue_blocking(query)) { // false if queue is empty or retry limit is reached
            return query;
        } else {
            return {};
        }
    }

    void PostgresManager::stop() {
        while (m_queries.size() > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        m_queue_thread_is_running = false;
    }

    void PostgresManager::run() {
        get_connection(m_conn_insert);
        while (m_queue_thread_is_running) {
            if (PQstatus(m_conn_insert.get()) != CONNECTION_OK) {
                m_logger->send<simple_logger::LogLevel::ERROR>(
                        "Connection to database failed: " + std::string(PQerrorMessage(m_conn_insert.get())));
                PQfinish(m_conn_insert.get());
                // sleep for 1 second
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                get_connection(m_conn_insert);
            }
            if (m_multi_insert) {
                std::vector<std::string> queries;
                while (m_queries.size() > 0) {
                    queries.push_back(dequeue());
                }
                if (!queries.empty()) {
                    if (!insert_multi(queries)) { // if insert fails, try individual m_insert
                        for (auto &query: queries) {
                            if (!m_insert(query)) // if m_insert fails, enqueue again
                                enqueue(query);
                        }
                    }
                }
            } else {
                std::string query = dequeue();
                if (!query.empty()) {
                    if (!m_insert(query)) // if m_insert fails, enqueue again
                        enqueue(query);
                }
            }
        }
        m_logger->send<simple_logger::LogLevel::DEBUG>("Queue thread stopped");
    }


    void PostgresManager::get_connection(std::shared_ptr<PGconn> &conn)  {
        PGconn* conn_raw = PQconnectdb(m_config.uri.c_str());

        if (PQstatus(conn_raw) != CONNECTION_OK) {
            PQfinish(conn_raw);
            m_logger->send<simple_logger::LogLevel::ERROR>(
                    "Get connection to database failed: " + std::string(PQerrorMessage(conn.get())));
        }
        conn = std::shared_ptr<PGconn>(conn_raw, [](PGconn* conn) {
            PQfinish(conn);
        });
    }



}
