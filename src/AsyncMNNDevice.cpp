#include <string>
#include <fcntl.h>
#include <iostream>

#include "../include/AsyncMNNDevice.hpp"
#include "../include/FileStream.hpp"
#include "../include/MNNStream.hpp"

namespace sgns::io{

    std::shared_ptr<IOStream> AsyncMNNDevice::open(AsioContext& io_context,
                                                    const std::string& path,
                                                    const IOStream::StreamDirection& dir,
                                                    const IOStream::StreamFlags& flags) {
        int fd = 0;

        if (dir == IOStream::StreamDirection::READ_ONLY) {
            fd = ::open(path.c_str(), O_RDONLY);

        } else if (dir == IOStream::StreamDirection::WRITE_ONLY) {
            fd = ::open(path.c_str(), O_WRONLY);
        }
        else{
            std::cout << "Opening AsyncMNNDevice is only allowed for READ_ONLY or WRITE_ONLY\n";
        }

        // the object is of type posix::stream_descriptor, which is a class provided by the
        // Boost.Asio library to represent a descriptor for an asynchronous stream based on a POSIX file descriptor.
        m_stream_file = std::make_shared<posix::stream_descriptor>(io_context, fd);
        std::shared_ptr<IOStream> stream = std::make_shared<MNNStream>(m_stream_file);
        return stream;
    }

    void AsyncMNNDevice::close() {

    }
}