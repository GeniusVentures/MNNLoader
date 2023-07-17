#ifndef SGNS_IPFSSTREAM_HPP
#define SGNS_IPFSSTREAM_HPP

namespace sgns::io {

    // Encapsulates an IPFS stream 
    class IPFSStream : public IOStream {
        
    public:
	IPFSStream();
	IPFSStream(IPFSStream&)

	
	void read(const char*, size_t bytes) override;
	size_t write(const char*, size_t bytes) override;
    private:
        // ipfs context 

    };

} 


#endif // SGNS_IPFSSTREAM_HPP
