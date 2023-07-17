#ifndef SGNS_FILEDEVICE_HPP
#define SGNS_FILEDEVICE_HPP

#include <string>
#include "IOStream.hpp"

namespace sgns::io {
    
    // Device for Streaming Local files
    // file:// schema
    class FileDevice: public IODevice {
    public:
	FileDevice();
	virtual ~FileDevice() = 0;

        // open the device
	IOStream& open(const string& path, const StreamDirection& dir, const StreamMode& mode) override;
	void close() override; 

    private:
	// ipfs lite cpp handler

    };

}

#endif // SGNS_IPFSDEVICE_HPP


