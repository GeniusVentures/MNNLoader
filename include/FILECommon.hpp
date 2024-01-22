/**
 * Header file for the FILECommon
 */
//#if defined(BOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR) //Can I use this?
#ifndef _WIN32
#include <fcntl.h>
#include "boost/asio/posix/stream_descriptor.hpp"
#endif
#include <iostream>
#include <memory>
#include "boost/asio.hpp"
#ifndef FILECOMMON_HPP
#define FILECOMMON_HPP

namespace sgns
{
    using namespace boost::asio;
#ifndef _WIN32
    class FILEDevice : public std::enable_shared_from_this<FILEDevice> {
    public:
        FILEDevice(std::shared_ptr<boost::asio::io_context> ioc,
            std::string filename, int writemode);
        ~FILEDevice() {
            // Cleanup
            file_.close(ec_);
            close(fd_);
        }
        boost::asio::posix::stream_descriptor& getFile() {
            return file_;
        }
    private:
        boost::asio::posix::stream_descriptor file_;
        boost::system::error_code ec_;
        int fd_ = -1;
    };
#else
    class FILEDevice : public std::enable_shared_from_this<FILEDevice> {
    public:
        FILEDevice(std::shared_ptr<boost::asio::io_context> ioc,
            std::string filename, int writemode);
        ~FILEDevice() {
            // Cleanup
            file_.close();
        }
        boost::asio::stream_file& getFile() {
            return file_;
        }
    private:
        boost::asio::stream_file file_;
        boost::system::error_code ec_;
    };
#endif

}

#endif