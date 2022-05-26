#include "socket/default-socket.hh"

#include <arpa/inet.h>
#include <iostream>
#include <utility>

#include "misc/socket.hh"

namespace http
{
    DefaultSocket::DefaultSocket(const misc::shared_fd &sock)
        : Socket{ sock }
    {
        try
        {
            sys::fcntl_set(*(fd_get()), O_NONBLOCK);
        }
        catch (...)
        {
            std::cerr << "ERROR: DefSock Constructor with fd.\n";
            exit(1);
        }
    }

    DefaultSocket::DefaultSocket(int domain, int type, int protocol)
        : Socket{ std::make_shared<misc::FileDescriptor>(
            sys::socket(domain, type, protocol)) }
    {
        try
        {
            sys::fcntl_set(*(fd_get()), O_NONBLOCK);
            ipv6_ = (domain == AF_INET6);
        }
        catch (...)
        {
            std::cerr << "ERROR: DefSock Constructor with 3ints.\n";
            exit(1);
        }
    }

    ssize_t DefaultSocket::recv(void *dst, size_t len)
    {
        try
        {
            return sys::recv(*fd_, dst, len, MSG_DONTWAIT);
        }
        catch (...)
        {
            std::cerr << "ERROR: DefSock recv.\n";
            exit(1);
        }
    }

    ssize_t DefaultSocket::send(const void *src, size_t len)
    {
        try
        {
            return sys::send(*fd_, src, len, MSG_DONTWAIT);
        }
        catch (...)
        {
            std::cerr << "ERROR: DefSock send.\n";
            exit(1);
        }
    }

    ssize_t DefaultSocket::sendfile(misc::shared_fd &fd, off_t &o, size_t len)
    {
        try
        {
            return sys::sendfile(*fd_, *fd, &o, len);
        }
        catch (...)
        {
            std::cerr << "ERROR: DefSock sendfile.\n";
            exit(1);
        }
    }

    void DefaultSocket::bind(const sockaddr *addr, socklen_t addrlen)
    {
        try
        {
            sys::bind(*fd_, addr, addrlen);
        }
        catch (...)
        {
            std::cerr << "ERROR: DefSock bind.\n";
            exit(1);
        }
    }

    void DefaultSocket::listen(int backlog)
    {
        try
        {
            sys::listen(*fd_, backlog);
        }
        catch (...)
        {
            std::cerr << "ERROR: DefSock listen.\n";
            exit(1);
        }
    }

    void DefaultSocket::setsockopt(int level, int optname, int optval)
    {
        try
        {
            sys::setsockopt(*fd_, level, optname, &optval, sizeof(optval));
        }
        catch (...)
        {
            std::cerr << "ERROR: DefSock setsockopt.\n";
            exit(1);
        }
    }

    void DefaultSocket::getsockopt(int level, int optname, int &optval)
    {
        try
        {
            sys::setsockopt(*fd_, level, optname, &optval, sizeof(optval));
        }
        catch (...)
        {
            std::cerr << "ERROR: DefSock getsockopt.\n";
            exit(1);
        }
    }

    shared_socket DefaultSocket::accept(sockaddr *addr, socklen_t *addrlen)
    {
        try
        {
            misc::FileDescriptor my_fd = sys::accept(*fd_, addr, addrlen);
            auto my_shared =
                std::make_shared<misc::FileDescriptor>(std::move(my_fd));
            auto socket = std::make_shared<DefaultSocket>(my_shared);
            sys::fcntl_set(*(socket->fd_get()), O_NONBLOCK);
            return socket;
        }
        catch (...)
        {
            std::cerr << "ERROR: DefSock accept.\n";
            exit(1);
        }
    }

    void DefaultSocket::connect(const sockaddr *addr, socklen_t addrlen)
    {
        try
        {
            sys::connect(*fd_, addr, addrlen);
        }
        catch (...)
        {
            std::cerr << "ERROR: DefSock connect.\n";
            exit(1);
        }
    }

} // namespace http
