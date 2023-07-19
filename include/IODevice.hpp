#ifndef SGNS_IODEVICE_HPP
#define SGNS_IODEVICE_HPP

#include <string>
#include "IOStream.hpp"

namespace sgns::io {

    class IODevice {

    public:
        virtual ~IODevice() {}

	// open for input/output/bidirectional
	virtual std::shared_ptr<IOStream> open(const std::string& path, 
			       const IOStream::StreamDirection& dir,
			       const IOStream::StreamFlags& flags) = 0;
	virtual void close() = 0; 

    private:
	// maintain all the opened streams

    };
    
}

#endif // IODevice

