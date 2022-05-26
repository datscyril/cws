/**
 * \file vhost/connection.hh
 * \brief Connection declaration.
 */

#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "misc/fd.hh"
#include "request/request.hh"
#include "socket/socket.hh"

#define DEFAULT_REMAINING -10000

namespace http
{
    using shared_fd = std::shared_ptr<misc::FileDescriptor>;
    /**
     * \struct Connection
     * \brief Value object representing a connection.
     *
     * We need to keep track of the state of each request while it has not
     * been fully processed.
     */
    struct Connection
    {
        Connection() = default;
        Connection(const Connection &con) = default;
        Connection &operator=(const Connection &) = default;
        Connection(Connection &&) = default;
        Connection &operator=(Connection &&) = default;
        ~Connection() = default;

        void field_get(http::Request &req);

        // Members for Recv
        std::string message_;
        int remaining_;
        bool field_received_;

        // Members for Send
        std::string res_;
        int offset_ = 0;
        shared_socket sock_;
        shared_socket file_;
        int file_len_ = 0;
    };
} // namespace http
