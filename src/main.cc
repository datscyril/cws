#include <arpa/inet.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "config/config.hh"
#include "error/not-implemented.hh"
#include "events/listener.hh"
#include "events/register.hh"
#include "misc/addrinfo/addrinfo.hh"
#include "misc/json.hh"
#include "misc/readiness/readiness.hh"
#include "misc/unistd.hh"
#include "socket/default-socket.hh"
#include "vhost/dispatcher.hh"
#include "vhost/vhost-factory.hh"
#include "vhost/vhost.hh"

static struct http::DefaultSocket
create_and_bind(const misc::AddrInfo &addrinfo)
{
    for (auto it = addrinfo.begin(); it != addrinfo.end(); it++)
    {
        try
        {
            auto dom = it->ai_family;
            auto type = it->ai_socktype;
            auto prot = it->ai_protocol;
            auto sockfd = http::DefaultSocket(dom, type, prot);
            int optval = 1;
            sockfd.setsockopt(SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, optval);
            sockfd.bind(it->ai_addr, it->ai_addrlen);
            return sockfd;
        }
        catch (...)
        {
            continue;
        }
    }
    throw std::runtime_error("create_and_bind");
}

static bool is_ipv6_address(const std::string &str)
{
    struct sockaddr_in6 sa;
    return inet_pton(AF_INET6, str.c_str(), &(sa.sin6_addr)) != 0;
}

static http::shared_socket create_socket(http::shared_vhost vh)
{
    try
    {
        misc::AddrInfoHint hints;
        int domain = (is_ipv6_address(vh->conf_get().ip_)) ? AF_INET6 : AF_INET;
        hints.family(domain);
        hints.socktype(SOCK_STREAM);
        hints.flags(AI_PASSIVE);
        auto ip = vh->conf_get().ip_.c_str();
        auto port = std::to_string(vh->conf_get().port_).c_str();
        misc::AddrInfo res = misc::getaddrinfo(ip, port, hints);

        auto x = create_and_bind(res);
        return std::make_shared<http::DefaultSocket>(std::move(x));
    }
    catch (...)
    {
        throw std::runtime_error("Error while creating ListenerEW");
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        std::cerr << "Usage: ./spider [-t] file.json\n";
        exit(1);
    }

    try
    {
        const std::string json_file((argc == 2) ? argv[1] : argv[2]);
        http::ServerConfig s = http::parse_configuration(json_file);
        if (argc == 3 && strcmp(argv[1], "-t") == 0)
            return 0;

        for (auto c : s.configs_)
            http::dispatcher.vhost_get().push_back(
                http::VHostFactory::Create(c));
        ev_signal sigint_w;
        http::event_register.loop_get().register_sigint_watcher(&sigint_w);
        http::event_register.register_event<http::ListenerEW>(
            create_socket(*(http::dispatcher.vhost_get().begin())));
        misc::announce_spider_readiness(argv[0]);
        http::event_register.loop_get()(); // Start the loop

        return 0;
    }
    catch (...)
    {
        std::cerr << "Tuez les tous, Dieu reconnaitra les siens\n";
        return 1;
    }
}
