#ifndef SGNS_IPFSDEVICE_HPP
#define SGNS_IPFSDEVICE_HPP

#include <string>

namespace sgns::io {
    
    // Device for Streaming to/from IPFS networks
    // ipfs:// schema
    class IPFSDevice: public IODevice {
    public:
	virtual ~IPFSDevice() = 0;

        // open the device
	IOStream& open(const string& path, const StreamDirection& dir, const StreamMode& mode) override;
	void close() override; 

    private:
	// ipfs lite cpp handler

    };


}

#endif // SGNS_IPFSDEVICE_HPP


