// IPFSLoader.cpp

#include "FileManager.hpp"
#include "IPFSLoader.hpp"

SINGLETON_PTR_INIT(IPFSLoader);

IPFSLoader::IPFSLoader() {
    FileManager::GetInstance().RegisterLoader("ipfs", this);
}
inline auto operator""_cid(const char* c, size_t s) {
    return sgns::CID::fromBytes(sgns::common::unhex(std::string_view(c, s)).value())
        .value();
}
inline std::vector<uint8_t> operator""_unhex(const char* c, size_t s) {
    return sgns::common::unhex(std::string_view(c, s)).value();
}
std::shared_ptr<void> IPFSLoader::LoadFile(std::string filename) {
    std::cout << "IPFSLOAD:" << filename << std::endl;
    std::shared_ptr<sgns::ipfs_lite::ipfs::IpfsBlockService> block_service;
    sgns::CID test{
        sgns::CID::Version::V1,
        libp2p::multi::MulticodecType::Code::SHA2_256,
        libp2p::multi::Multihash::create(libp2p::multi::HashType::sha256,
            "0123456789ABCDEF0123456789ABCDEF"_unhex).value()
    };
    const char *dummyValue = "Inside the IPFSLoader::LoadFile Function";
    // for this test, we don't need to delete the shared_ptr as the data is static, so pass null lambda delete function
    return {(void *)dummyValue, [](void *) {} };
    /* TODO: scorpioluck20 - Need to implement this. How we load file base on format file?*/
}
std::shared_ptr<void> IPFSLoader::LoadASync(std::string filename, bool parse)
{
    const char* dummyValue = "Inside the IPFSLoader::LoadFile Function";
    // for this test, we don't need to delete the shared_ptr as the data is static, so pass null lambda delete function
    return { (void*)dummyValue, [](void*) {} };
    /* TODO: scorpioluck20 - Need to implement this. How we load file base on format file?*/
}

