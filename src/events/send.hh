/**
 * \file events/receive.hh
 * \brief SendResponseEW declaration.
 */

#pragma once

#include "events/events.hh"
#include "request/response.hh"
#include "socket/default-socket.hh"
#include "socket/socket.hh"
#include "vhost/connection.hh"

namespace http
{
    /**
     * \class SendResponseEW
     * \brief Workflow for listener socket.
     */
    class SendResponseEW : public EventWatcher
    {
    public:
        /**
         * \brief Create a SendResponseEW from a listener socket.
         */
        explicit SendResponseEW(shared_socket socket, Response resp);

        /**
         * \brief Start receiving requests.
         */
        void operator()() final;
        shared_socket sock_get();

        std::shared_ptr<Connection> get_con();

        Response get_resp();

        uint16_t get_port();

    private:
        /**
         * \brief SendResponse socket.
         */
        shared_socket sock_;
        /**
         * \brief Port on which the socket is listening.
         */
        uint16_t port_;
        /**
         * \brief The connection
         */
        std::shared_ptr<Connection> con_;
        Response resp_;
    };
} // namespace http
