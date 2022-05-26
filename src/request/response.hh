/**
 * \file request/response.hh
 * \brief Response declaration.
 */

#pragma once

#include "request/request.hh"
#include "request/types.hh"

namespace http
{
    /**
     * \struct Response
     * \brief Value object representing a response.
     */
    struct Response
    {
        explicit Response(const STATUS_CODE &);
        Response(const Request &, const STATUS_CODE & = STATUS_CODE::OK);

        Response() = default;
        Response(const Response &) = default;
        Response &operator=(const Response &) = default;
        Response(Response &&) = default;
        Response &operator=(Response &&) = default;
        ~Response() = default;

        void init_msg();
        void init_msg(int flen);

        // Members to store the information relative to a response.
        STATUS_CODE code_;
        http::Request reqt_ = http::Request();
        std::string msg_;
        bool is_head_ = false;
        std::string file_name_ = "";
    };
} // namespace http
