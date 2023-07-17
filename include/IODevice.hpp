#ifndef SGNS_IODEVICE_HPP
#define SGNS_IODEVICE_HPP

#include <string>

namespace sgns::io {

    class IODevice {

    public:
        virtual ~IODevice() {}

	// open for input/output/bidirectional
	virtual IOStream& open(const std::string& path, 
			       const StreamDirection& dir,
			       const StreamFlags& flags) = 0;
	virtual void close() = 0; 

    private:
	// maintain all the opened streams

    };
    
}

#endif // IODevice

