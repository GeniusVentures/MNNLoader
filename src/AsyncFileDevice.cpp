#include <iostream>
#include "AsyncFileDevice.hpp"
#include "FileStream.hpp"
#include "IOException.hpp"
#include <boost/asio.hpp>

using namespace std;

namespace sgns::io {
    
    AsyncFileDevice::AsyncFileDevice() {

    }

    std::shared_ptr<IOStream> AsyncFileDevice::open(AsioContext& io_context,
		                                    const string& path,
		                                    const IOStream::StreamDirection& dir,
		                                    const IOStream::StreamFlags& flags) {
	int fd = 0;

	if (dir == IOStream::StreamDirection::READ_ONLY) {
            fd = ::open(path.c_str(), O_RDONLY);		
	} else if (dir == IOStream::StreamDirection::WRITE_ONLY) {
            fd = ::open(path.c_str(), O_WRONLY);		
	}
	stream_file_ = std::make_shared<posix::stream_descriptor>(io_context, fd);
	std::shared_ptr<IOStream> stream = std::make_shared<FileStream>(stream_file_);
	return stream;
    }

    void AsyncFileDevice::close() {

    }

}
