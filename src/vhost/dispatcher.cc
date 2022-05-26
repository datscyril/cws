#include "dispatcher.hh"

#include <regex>

#include "events/register.hh"
#include "events/send.hh"

namespace http
{
    Dispatcher dispatcher;

    std::vector<shared_vhost> &Dispatcher::vhost_get()
    {
        return vhosts_;
    }

    void Dispatcher::vhost_set(std::vector<shared_vhost> &vhost)
    {
        vhosts_ = vhost;
    }

    static bool check_request(const http::Request req)
    {
        // Check that there is no trailing WS in names
        for (auto i : req.fields_)
        {
            std::string field = i.first;
            if (field[field.size() - 1] == ' '
                || field[field.size() - 1] == '\t')
                return false;
        }

        // Check if reqt has Host header
        auto it = req.fields_.find("Host");
        if (it == req.fields_.end())
            return false;

        // Check if reqt has a valid Content-Length header
        it = req.fields_.find("Content-Length");
        if (it != req.fields_.end())
        {
            try
            {
                int cl = std::stoi(it->second);
                if (cl < 0)
                    return false;
            }
            catch (...)
            {
                return false;
            }
        }
        return true;
    }

    static bool is_vhost_ok(std::string host, shared_vhost vh)
    {
        std::string port = ":" + std::to_string(vh->conf_get().port_);
        std::string name = vh->conf_get().name_;
        std::string name_port = name + port;
        std::string ip = vh->conf_get().ip_;
        std::string ip_port = ip + port;
        return (host.compare(name) == 0 || host.compare(name_port) == 0
                || host.compare(ip) == 0 || host.compare(ip_port) == 0);
    }

    static bool all_methods(std::string m)
    {
        if (m.compare("GET") == 0)
            return true;
        if (m.compare("POST") == 0)
            return true;
        if (m.compare("HEAD") == 0)
            return true;
        if (m.compare("PUT") == 0)
            return true;
        if (m.compare("DELETE") == 0)
            return true;
        if (m.compare("CONNECT") == 0)
            return true;
        if (m.compare("OPTIONS") == 0)
            return true;
        if (m.compare("TRACE") == 0)
            return true;
        return false;
    }
    static bool good_methods(std::string m)
    {
        return (m.compare("GET") == 0 || m.compare("POST") == 0
                || m.compare("HEAD") == 0);
    }

    static void error_400(http::RecvRequestEW *r)
    {
        Response resp = http::Response(http::error::bad_request());
        resp.init_msg();
        http::event_register.register_event<http::SendResponseEW>(r->sock_get(),
                                                                  resp);
    }

    static void error_405(http::RecvRequestEW *r, http::Request reqt)
    {
        Response x = http::Response(http::error::method_not_allowed(reqt));
        x.init_msg();
        http::event_register.register_event<http::SendResponseEW>(r->sock_get(),
                                                                  x);
    }
    static void error_426(http::RecvRequestEW *r, http::Request reqt)
    {
        Response x = http::Response(http::error::upgrade_required(reqt));
        x.init_msg();
        http::event_register.register_event<http::SendResponseEW>(r->sock_get(),
                                                                  x);
    }
    void Dispatcher::dispatch(http::Request reqt, http::RecvRequestEW *r)
    {
        std::cout << "\n=== Got  REQUEST ===\n";
        std::cout << "Methods: " << reqt.method_ << "\n";
        std::cout << "Version: " << reqt.version_ << "\n";
        std::cout << "Body: " << reqt.body_ << "\n";
        if (reqt.mult_content_length)
        {
            std::cout << "Mult length\n";
            error_400(r);
            return;
        }
        if (!good_methods(reqt.method_) && all_methods(reqt.method_))
        {
            error_405(r, reqt);
            return;
        }
        else if (!all_methods(reqt.method_))
        {
            std::cout << "Wrong method\n";
            error_400(r);
            return;
        }
        try
        {
            std::regex regex("HTTP/[0-9]\\.[0-9]");
            if (!regex_match(reqt.version_, regex))
            {
                std::cout << "Not \"HTTP\"\n";
                error_400(r);
                return;
            }
            double vers = stod(reqt.version_.substr(5, 6));
            if (vers != 1.1)
            {
                error_426(r, reqt);
                return;
            }
        }
        catch (...)
        {
            std::cout << "Error while creating double\n";
            error_400(r);
            return;
        }
        if (check_request(reqt))
        {
            std::string host = reqt.fields_.find("Host")->second;
            for (auto vh : vhosts_)
            {
                if (is_vhost_ok(host, vh))
                {
                    vh->respond(reqt, r->get_con());
                    return;
                }
            }
        }
        std::cout << "End of fct\n";
        error_400(r);
    }
} // namespace http
