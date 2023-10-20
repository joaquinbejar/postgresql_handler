//
// Created by Joaquin Bejar Garcia on 20/10/23.
//
#include "postgresql/config.h"

namespace postgresql::config {

    bool PostgresqlConfig::validate() {
        if (m_hostname.empty()) {
            logger.error("Hostname is empty");
        }
        if (common::ip::is_a_valid_port(m_port)) {
            logger.error("Port is not valid");
        }
        if (m_user.empty()) {
            logger.error("User is empty");
        }
        if (m_password.empty()) {
            logger.error("Password is empty");
        }
        if (m_database.empty()) {
            logger.error("Database is empty");
        }

        return true;
    }

    json PostgresqlConfig::to_json() const {
        json j;
        j["m_hostname"] = m_hostname;
        j["m_port"] = m_port;
        j["m_user"] = m_user;
        j["m_password"] = m_password;
        j["dbname"] = m_database;
        return j;
    }

    void PostgresqlConfig::from_json(const json &j) {
        try {
            m_hostname = j.at("m_hostname").get<std::string>();
            m_port = j.at("m_port").get<int>();
            m_user = j.at("m_user").get<std::string>();
            m_password = j.at("m_password").get<std::string>();
            m_database = j.at("dbname").get<std::string>();
        } catch (std::exception &e) {
            throw simple_config::ConfigException(e.what());
        }
    }

    std::string PostgresqlConfig::to_string() const {
        return (std::string) "PostgresqlConfig{" +
               "m_hostname=" + m_hostname +
               ", m_port=" + std::to_string(m_port) +
               ", m_user=" + m_user +
               ", m_password=" + m_password +
               ", m_database=" + m_database +
               '}';
    }

}
