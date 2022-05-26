#include "misc/fd.hh"

#include <iostream>

#include "misc/unistd.hh"

namespace misc
{
    FileDescriptor::~FileDescriptor()
    {
        try
        {
            if (fd_ != -1)
            {
                std::cout << "Suppressing: " << fd_ << "\n";
                sys::close(fd_);
                // std::cout << "Suppressed\n";
            }
        }
        catch (...)
        {
            std::cerr << "Yo bg\n";
            exit(21);
        }
    }

    FileDescriptor &FileDescriptor::operator=(FileDescriptor &&fileDescriptor)
    {
        if (fd_ != -1)
            sys::close(fd_);
        fd_ = std::exchange(fileDescriptor.fd_, -1);
        return *this;
    }

    FileDescriptor::operator int() const &
    {
        return fd_;
    }

    FileDescriptor::operator bool() const &
    {
        return (fcntl(fd_, F_GETFD) != -1 || errno != EBADF);
    }

} // namespace misc
