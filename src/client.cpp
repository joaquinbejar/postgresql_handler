//
// Created by Joaquin Bejar Garcia on 20/10/23.
//

#include "postgresql/client.h"

namespace postgresql::client {

    PostgresManager::PostgresManager(postgresql::config::PostgresqlConfig &config) : m_config(config) {
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


}
