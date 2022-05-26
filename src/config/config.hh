/**
 * \file config/config.hh
 * \brief Declaration of ServerConfig and VHostConfig.
 */

#pragma once

#include <string>

#include "misc/json.hh"

namespace http
{
    struct VHostConfig;
    using list_vhostconfig = std::vector<VHostConfig>;
    /**
     * \struct VHostConfig
     * \brief Value object storing a virtual host configuration.
     *
     * Since each virtual host of the server has its own configuration, a
     * dedicated structure is required to store the information related to
     * each one of them.
     */
    struct VHostConfig
    {
        VHostConfig() = default;
        VHostConfig(const VHostConfig &) = default;
        VHostConfig &operator=(const VHostConfig &) = default;
        VHostConfig(VHostConfig &&) = default;
        VHostConfig &operator=(VHostConfig &&) = default;

        ~VHostConfig() = default;

        // Members to store the information relative to a vhost.
        std::string ip_;
        uint16_t port_;
        std::string name_;
        std::string default_file_;
        std::string root_;
    };

    /**
     * \struct ServerConfig
     * \brief Value object storing the server configuration.
     *
     * To avoid opening the configuration file each time we need to access the
     * server configuration, a dedicated structure is required to store it.
     */
    struct ServerConfig
    {
        ServerConfig() = default;
        ServerConfig(const ServerConfig &) = default;
        ServerConfig &operator=(const ServerConfig &) = default;
        ServerConfig(ServerConfig &&) = default;
        ServerConfig &operator=(ServerConfig &&) = default;

        ~ServerConfig() = default;

        /* Members to store the information relative to the
           configuration of the server. */
        list_vhostconfig configs_;
    };

    /**
     * \brief Parse the server configuration file.
     *
     * \param path string containing the path to the server configuration
     * file.
     * \return The server configuration.
     */
    struct ServerConfig parse_configuration(const std::string &path);

} // namespace http
