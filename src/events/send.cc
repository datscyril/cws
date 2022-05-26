#include "send.hh"

#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "events/register.hh"
#include "misc/unistd.hh"
#include "socket/default-socket.hh"

namespace fs = std::filesystem;

namespace http
{
    SendResponseEW::SendResponseEW(shared_socket socket, Response resp)
        : EventWatcher(*(socket->fd_get()), EV_WRITE)
        , sock_(socket)
        , resp_(resp)
    {
        struct sockaddr_in sin;
        socklen_t len = sizeof(sin);

        try
        {
            std::cout << "Is_head: " << std::boolalpha << resp.is_head_ << "\n";
            std::cout << "File_name: \"" << resp.file_name_ << "\"\n";
            sys::getsockname(*(socket->fd_get()), (struct sockaddr *)&sin,
                             &len);
            port_ = sin.sin_port;
            con_ = std::make_shared<http::Connection>();

            int flen = 0;
            shared_fd file = std::make_shared<misc::FileDescriptor>(
                sys::open(resp.file_name_.c_str(), O_RDONLY));
            if (resp.is_head_)
            {
                sys::close(*file);
                file = nullptr;
                resp_.init_msg();
            }
            else
            {
                fs::path p(resp.file_name_);
                flen = fs::file_size(p);
                resp_.init_msg(flen);
            }

            if (file != nullptr)
                con_->file_ = std::make_shared<http::DefaultSocket>(file);
            else
                con_->file_ = nullptr;
            con_->file_len_ = flen;
        }
        catch (...)
        {
            exit(1);
        }
    }

    void SendResponseEW::operator()()
    {
        // Sending headers
        if (resp_.msg_.size() != 0)
        {
            ssize_t l = sock_->send(resp_.msg_.c_str(), resp_.msg_.size());
            resp_.msg_ = resp_.msg_.substr(l, resp_.msg_.size());
            if (resp_.msg_.size() != 0)
                return;
        }

        // Sending body
        if (con_->offset_ < con_->file_len_ && !resp_.is_head_
            && con_->file_ != nullptr)
        {
            std::cout << "Offset: " << con_->offset_ << "\n";
            std::cout << "File_len: " << con_->file_len_ << "\n";
            int len = con_->file_len_ - con_->offset_;
            // auto x = sock_->fd_get();
            off_t off = con_->offset_;
            // ssize_t l = con_->file_->sendfile(x, off, len);
            auto x1 = con_->file_->fd_get();
            ssize_t l = sock_->sendfile(x1, off, len);
            con_->offset_ += l;
            if (con_->offset_ < con_->file_len_)
                return;
        }

        http::event_register.unregister_ew(this);
    }

    shared_socket SendResponseEW::sock_get()
    {
        return sock_;
    }

    std::shared_ptr<Connection> SendResponseEW::get_con()
    {
        return con_;
    }

    Response SendResponseEW::get_resp()
    {
        return resp_;
    }

    uint16_t SendResponseEW::get_port()
    {
        return port_;
    }
} // namespace http
