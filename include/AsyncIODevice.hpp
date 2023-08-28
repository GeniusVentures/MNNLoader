#ifndef SGNS_ASYNC_IODEVICE_HPP
#define SGNS_ASYNC_IODEVICE_HPP

#include <string>
#include <boost/asio.hpp>
#include "IOStream.hpp"

namespace sgns::io {

    class AsyncIODevice {

    public:
        virtual ~AsyncIODevice() {}

	// open for input/output/bidirectional
	virtual std::shared_ptr<IOStream> open(boost::asio::io_context io_context,
			       const std::string& path, 
			       const IOStream::StreamDirection& dir,
			       const IOStream::StreamFlags& flags) = 0;
	virtual void close() = 0; 

    private:
	// maintain all the opened streams

    };
    
}

#endif // IODevice

