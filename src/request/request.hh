/**
 * \file request/request.hh
 * \brief Request declaration.
 */

#pragma once

#include <map>
#include <sstream>

#include "request/types.hh"

namespace http
{
    /**
     * \struct Request
     * \brief Value object representing a request.
     */
    struct Request
    {
        Request() = default;
        Request(const Request &) = default;
        Request &operator=(const Request &) = default;
        Request(Request &&) = default;
        Request &operator=(Request &&) = default;
        ~Request() = default;

        std::map<std::string, std::string> fields_;
        std::string method_;
        std::string version_;
        std::string uri_;
        std::string body_; // the one that we will get with Connection
        bool mult_content_length = false;
    };
} // namespace http
