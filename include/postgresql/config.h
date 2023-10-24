//
// Created by Joaquin Bejar Garcia on 20/10/23.
//

#ifndef POSTGRESQL_HANDLER_CONFIG_H
#define POSTGRESQL_HANDLER_CONFIG_H

#include "postgresql/exceptions.h"
#include <simple_config/config.h>
#include <simple_logger/logger.h>
#include <common/common.h>
#include <common/ip.h>

using json = nlohmann::json;

namespace postgresql::config {

    class PostgresqlConfig : public simple_config::Config {
    public:

        bool validate() override;

        [[nodiscard]] json to_json() const override;

        void from_json(const json &j) override;

        [[nodiscard]] std::string to_string() const override;

        bool multi_insert = common::get_env_variable_bool("PG_MULTI_INSERT", false);

    protected:
        std::string m_database = common::get_env_variable_string("PG_DATABASE", "");
        std::string m_password = common::get_env_variable_string("PG_PASSWORD", "");
        std::string m_hostname = common::get_env_variable_string("PG_HOSTNAME", "");
        std::string m_user = common::get_env_variable_string("PG_USER", "");
        int m_port = common::get_env_variable_int("PG_PORT", 5432);


    public:
        std::string uri =
                "postgresql://" + m_user + ":" + m_password + "@" + m_hostname + ":" + std::to_string(m_port) + "/" +
                m_database;
        std::shared_ptr<simple_logger::Logger> logger = std::make_shared<simple_logger::Logger>(loglevel);

    };
}
#endif //POSTGRESQL_HANDLER_CONFIG_H
