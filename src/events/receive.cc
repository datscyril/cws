#include "events/receive.hh"

#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "events/register.hh"
#include "misc/socket.hh"
#include "socket/default-socket.hh"
#include "vhost/dispatcher.hh"

#define BUFFER_SIZE 4096

namespace http
{
    RecvRequestEW::RecvRequestEW(shared_socket socket)
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
            con_->message_ = "";
            con_->remaining_ = DEFAULT_REMAINING;
            con_->field_received_ = false;
            con_->sock_ = socket;
            reqt_.body_ = "";
        }
        catch (...)
        {
            exit(1);
        }
    }

    std::shared_ptr<Connection> RecvRequestEW::get_con()
    {
        return con_;
    }

    Request RecvRequestEW::get_reqt()
    {
        return reqt_;
    }

    uint16_t RecvRequestEW::get_port()
    {
        return port_;
    }

    void RecvRequestEW::operator()()
    {
        char buff[BUFFER_SIZE];
        ssize_t len = sock_->recv(buff, BUFFER_SIZE);
        if (!len)
        {
            http::event_register.unregister_ew(this);
            std::cout << "Client disconnected \n";
            return;
        }
        std::string msg(buff, len);
        if (con_->field_received_)
        {
            reqt_.body_ += msg;
            con_->remaining_ -= len;
            if (con_->remaining_ < 1 && con_->remaining_ != DEFAULT_REMAINING)
            {
                // dispatcher
                http::dispatcher.dispatch(reqt_, this);
                http::event_register.unregister_ew(this);
            }
        }
        else
        {
            con_->message_ += msg;
            con_->field_get(reqt_);
            if (con_->field_received_ && con_->remaining_ < 1)
            {
                // dispatcher
                http::dispatcher.dispatch(reqt_, this);
                http::event_register.unregister_ew(this);
            }
        }
    }

    shared_socket RecvRequestEW::sock_get()
    {
        return sock_;
    }

} // namespace http
