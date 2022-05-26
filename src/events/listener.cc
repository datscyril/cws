#include "events/listener.hh"

#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "events/receive.hh"
#include "events/register.hh"
#include "misc/socket.hh"
#include "socket/default-socket.hh"

namespace http
{
    ListenerEW::ListenerEW(shared_socket socket)
        : EventWatcher(*(socket->fd_get()), EV_READ)
        , sock_(socket)
    {
        struct sockaddr_in sin;
        socklen_t len = sizeof(sin);
        try
        {
            sys::getsockname(*(socket->fd_get()), (struct sockaddr *)&sin,
                             &len);
            port_ = sin.sin_port;
            sock_->listen(5);
        }
        catch (...)
        {
            exit(1);
        }
    }

    void ListenerEW::operator()()
    {
        try
        {
            struct sockaddr addr;
            socklen_t addrlen = sizeof(addr);
            auto s = sock_->accept(&addr, &addrlen);
            int si = *(s->fd_get());
            if (si != -1)
            {
                std::cout << "C1ient Connected: " << si << "\n";
                int opv = 1;
                s->setsockopt(SOL_SOCKET, SO_REUSEADDR, opv);
                http::event_register.register_event<http::RecvRequestEW>(s);
            }
        }
        catch (...)
        {
            std::cerr << "ERROR while Listener operator\n";
            exit(1);
        }
    }

    shared_socket ListenerEW::sock_get()
    {
        return sock_;
    }
} // namespace http
