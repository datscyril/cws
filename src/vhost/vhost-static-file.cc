#include "vhost/vhost-static-file.hh"

#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

#include "request/error.hh"

namespace fs = std::filesystem;

namespace http
{
    VHostStaticFile::VHostStaticFile(const VHostConfig &c)
        : VHost(c)
    {}

    static bool is_found(std::string file_name)
    {
        fs::file_status s = fs::status(file_name);
        return fs::exists(s);
    }

    static bool got_perms(std::string file_name)
    {
        fs::perms p = fs::status(file_name).permissions();
        return ((p & fs::perms::others_read) != fs::perms::none);
    }

    static void error_404(Request reqt, std::shared_ptr<Connection> con)
    {
        Response resp = http::Response(http::error::not_found(reqt));
        resp.init_msg();
        http::event_register.register_event<http::SendResponseEW>(con->sock_,
                                                                  resp);
    }

    static void error_403(Request reqt, std::shared_ptr<Connection> con)
    {
        Response resp = http::Response(http::error::forbidden(reqt));
        resp.init_msg();
        http::event_register.register_event<http::SendResponseEW>(con->sock_,
                                                                  resp);
    }

    /**
     * \brief Send response.
     *
     * \param req Request.
     * \param conn Connection.
     *
     * Note that these iterators will only be useful starting from SRPS.
     */
    void VHostStaticFile::respond(Request &reqt,
                                  std::shared_ptr<Connection> con)
    {
        // Build Response
        // Mettre le fichier dans le body
        // Enregistrer le SendResponceEW

        std::string full_path = conf_.root_;
        // if (full_path[full_path.size() - 1] != '/' && reqt.uri_.compare("/")
        // == 0)
        //    full_path += '/';
        // full_path += (reqt.uri_.compare("/") == 0) ? conf_.default_file_ :
        // reqt.uri_;
        full_path += reqt.uri_;
        if (full_path[full_path.size() - 1] == '/')
            full_path += conf_.default_file_;
        if (!is_found(full_path.c_str()))
        {
            error_404(reqt, con);
            return;
        }

        if (!got_perms(full_path.c_str()))
        {
            error_403(reqt, con);
            return;
        }

        Response resp = http::Response(http::STATUS_CODE::OK);
        resp.is_head_ = (reqt.method_.compare("HEAD") == 0);
        resp.file_name_ = full_path;
        http::event_register.register_event<http::SendResponseEW>(con->sock_,
                                                                  resp);
    }

} // namespace http
