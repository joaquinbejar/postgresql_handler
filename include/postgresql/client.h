//
// Created by Joaquin Bejar Garcia on 20/10/23.
//

#ifndef POSTGRESQL_HANDLER_CLIENT_H
#define POSTGRESQL_HANDLER_CLIENT_H


#include <mutex>
#include <simple_color/color.h>
#include <simple_config/config.h>
#include <simple_logger/logger.h>
#include <libpq/libpq-fe.h>
#include <postgresql/config.h>


namespace postgresql::client {
    class PostgresManager {
    public:
        explicit PostgresManager(postgresql::config::PostgresqlConfig &config);

        ~PostgresManager();

        bool insert(const std::string &query);

        bool insert_multi(const std::vector<std::string> &queries);

        std::vector<std::map<std::string, std::string>> select(const std::string &query);


    private:
        PGconn *conn;
        std::mutex db_mutex;
        postgresql::config::PostgresqlConfig &m_config;
        std::shared_ptr<simple_logger::Logger> m_logger = m_config.logger;
    };

}

#endif //POSTGRESQL_HANDLER_CLIENT_H
