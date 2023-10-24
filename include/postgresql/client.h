//
// Created by Joaquin Bejar Garcia on 20/10/23.
//

#ifndef POSTGRESQL_HANDLER_CLIENT_H
#define POSTGRESQL_HANDLER_CLIENT_H


#include <mutex>
#include <simple_color/color.h>
#include <simple_config/config.h>
#include <simple_logger/logger.h>
#include <libpq-fe.h>
#include <postgresql/config.h>
#include <regex>
#include <common/common.h>

namespace postgresql::client {

    bool is_insert_or_replace_query_correct(const std::string& query);

    class PostgresManager {
    public:
        explicit PostgresManager(postgresql::config::PostgresqlConfig &config);

        ~PostgresManager();

        bool insert(const std::string &query);

        bool insert_multi(const std::vector<std::string> &queries);

        std::vector<std::map<std::string, std::string>> select(const std::string &query);

        bool enqueue(const std::string &query);

        size_t queue_size();

        std::string dequeue();

        void stop();

        void run();

    private:
        PGconn *conn;
        std::mutex db_mutex;
        postgresql::config::PostgresqlConfig &m_config;
        std::shared_ptr<simple_logger::Logger> m_logger = m_config.logger;
        common::ThreadQueue<std::string> m_queries;
        std::atomic<bool> m_queue_thread_is_running;
        std::thread m_queue_thread;
        bool m_multi_insert = m_config.multi_insert;

    };

}

#endif //POSTGRESQL_HANDLER_CLIENT_H
