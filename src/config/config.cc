#include "config.hh"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "misc/json.hh"

namespace http
{
    static void from_json(const json &j, http::VHostConfig &x)
    {
        j.at("ip").get_to(x.ip_);
        j.at("port").get_to(x.port_);
        j.at("server_name").get_to(x.name_);
        j.at("root").get_to(x.root_);

        x.default_file_ = "index.html";
        if (j.contains("default_file"))
            j.at("default_file").get_to(x.default_file_);
    }

    static void from_json(const json &j, list_vhostconfig &conf)
    {
        for (auto vh : j["vhosts"])
        {
            conf.push_back(vh.get<http::VHostConfig>());
        }
    }

    struct ServerConfig parse_configuration(const std::string &path)
    {
        std::ifstream file(path);
        http::ServerConfig res;

        try
        {
            json j = json::parse(file);
            auto configs = j.get<list_vhostconfig>();
            res.configs_ = configs;
        }
        catch (...)
        {
            std::cerr << "Error while parsing " << path << ". ";
            std::cerr << "Please check the file format.\n";
            exit(1);
        }

        return res;
    }
} // namespace http
