/**
 * \file vhost/dispatcher.hh
 * \brief Dispatcher declaration.
 */

#pragma once

#include <vector>

#include "events/receive.hh"
#include "request/error.hh"
#include "vhost/vhost-static-file.hh"
#include "vhost/vhost.hh"

namespace http
{
    /**
     * \class Dispatcher
     * \brief Instance in charge of dispatching requests between vhosts.
     */

    class Dispatcher
    {
    public:
        Dispatcher() = default;
        Dispatcher(const Dispatcher &) = delete;
        Dispatcher &operator=(const Dispatcher &) = delete;
        Dispatcher(Dispatcher &&) = delete;
        Dispatcher &operator=(Dispatcher &&) = delete;

        // GETTER
        std::vector<shared_vhost> &vhost_get();
        // SETTER
        void vhost_set(std::vector<shared_vhost> &vhost);
        // CALL DISPATCHER
        void dispatch(http::Request reqt, http::RecvRequestEW *r);

    private:
        /* Add members to store the information relative to the
        ** Dispatcher.
        */
        std::vector<shared_vhost> vhosts_;
    };

    /**
     * \brief Service object.
     */
    extern Dispatcher dispatcher;
} // namespace http
