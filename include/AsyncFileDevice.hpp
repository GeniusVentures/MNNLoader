#ifndef SGNS_ASYNC_FILEDEVICE_HPP
#define SGNS_ASYNC_FILEDEVICE_HPP

#include <string>
#include <boost/asio.hpp>
#include "AsyncIODevice.hpp"
#include "IOStream.hpp"

using namespace boost::asio;

namespace sgns::io {
    
    // Device for Streaming Local files
    // file:// schema
    class AsyncFileDevice: public AsyncIODevice {
    public:
	using AsioContext = boost::asio::io_context;

	AsyncFileDevice();
	virtual ~AsyncFileDevice() {}

    // open the device
	std::shared_ptr<IOStream> open(AsioContext& io_context,
		       const std::string& path, 
		       const IOStream::StreamDirection& dir, 
		       const IOStream::StreamFlags& flags) override;
	void close() override; 

    private:
        std::shared_ptr<posix::stream_descriptor> stream_file_;
    };

}

#endif // SGNS_ASYNC_FILEDEVICE_HPP


