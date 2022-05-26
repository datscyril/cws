#include "request/response.hh"

#include <ctime>
#include <iostream>
#include <sstream>

namespace http
{
    Response::Response(const STATUS_CODE &c)
        : code_(c)
        , msg_("")
    {}

    Response::Response(const Request &r, const STATUS_CODE &c)
        : code_(c)
        , reqt_(r)
        , msg_("")
    {}

    static void do_header(std::string &msg, STATUS_CODE c, std::string version)
    {
        std::pair<STATUS_CODE, decltype(http_crlf)> p = statusCode(c);
        msg += (version.compare("") == 0) ? "HTTP/1.1" : version;
        msg += " ";
        msg += std::to_string(static_cast<uint16_t>(p.first));
        msg += " ";
        msg += p.second;
        msg += http::http_crlf;
        if (c == UPGRADE_REQUIRED)
        {
            msg += "Upgrade: HTTP/1.1\r\n";
        }
        // Date
        msg += "Date: ";
        time_t rawtime;
        struct tm *timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        std::string s = asctime(timeinfo);
        s.insert(3, ",");
        s = s.substr(0, s.size() - 1);
        msg += s;
        msg += " GMT";
        msg += http::http_crlf;
        // Connection
        msg += "Connection: close\r\n";
    }

    void Response::init_msg()
    {
        do_header(msg_, code_, reqt_.version_);
        msg_ += "Content-Length: 0\r\n";
        msg_ += http::http_crlf;
    }

    void Response::init_msg(int flen)
    {
        do_header(msg_, code_, reqt_.version_);
        msg_ += "Content-Length: ";
        msg_ += std::to_string(flen);
        msg_ += "\r\n";
        msg_ += http::http_crlf;
    }
} // namespace http
