/**
 * \file events/receive.hh
 * \brief RecvRequestEW declaration.
 */

#pragma once

#include "events/events.hh"
#include "request/request.hh"
#include "socket/socket.hh"
#include "vhost/connection.hh"

namespace http
{
    /**
     * \class RecvRequestEW
     * \brief Workflow for listener socket.
     */
    class RecvRequestEW : public EventWatcher
    {
    public:
        /**
         * \brief Create a RecvRequestEW from a listener socket.
         */
        explicit RecvRequestEW(shared_socket socket);

        /**
         * \brief Start receiving requests.
         */
        void operator()() final;
        shared_socket sock_get();

        std::shared_ptr<Connection> get_con();

        Request get_reqt();

        uint16_t get_port();

    private:
        /**
         * \brief RecvRequest socket.
         */
        shared_socket sock_;
        /**
         * \brief Port on which the socket is listening.
         */
        uint16_t port_;
        /**
         * \brief The connection
         */
        std::shared_ptr<Connection> con_ = std::make_shared<Connection>();
        Request reqt_ = Request();
    };
} // namespace http
