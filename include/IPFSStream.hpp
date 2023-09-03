#ifndef SGNS_IPFSSTREAM_HPP
#define SGNS_IPFSSTREAM_HPP

#include <boost/iostreams/categories.hpp>
#include <queue>
#include <mutex>
#include "ipfs_lite/ipfs/impl/ipfs_block_service.hpp"

//TODO: Remove this define in the testing phase.
#define EXPECTED_CHUNK_SIZE 1024 // This "define" is used in the write method, and added for the sake of development purposes.

namespace sgns::io
{

// Encapsulates an IPFS stream
class IPFSStream : public IOStream
{
public:
    using char_type = char;
    using category = boost::iostreams::seekable_device_tag;

    explicit IpfsStreamDevice(std::shared_ptr<IpfsBlockService> block_service);

    std::streamsize read(char* s, std::streamsize n);
    std::streamsize write(const char* s, std::streamsize n);

private:
    std::shared_ptr<IpfsBlockService> block_service_;
    std::queue<CID> cid_queue_;
    std::mutex mtx_;
    std::vector<uint8_t> read_buffer_;
    std::vector<uint8_t> write_buffer_;
};

} // namespace sgns::io

#endif // SGNS_IPFSSTREAM_HPP
