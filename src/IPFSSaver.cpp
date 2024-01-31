// IPFSLoader.cpp

#include <iostream>
#include "FileManager.hpp"
#include "IPFSSaver.hpp"
#include "libp2p/multi/content_identifier_codec.hpp"
#include "common/hexutil.hpp"

namespace sgns
{
    IPFSSaver* IPFSSaver::_instance = nullptr;
    void IPFSSaver::InitializeSingleton() {
        if (_instance == nullptr) {
            _instance = new IPFSSaver();
        }
    }

    IPFSSaver::IPFSSaver() {
        FileManager::GetInstance().RegisterSaver("ipfs", this);
    }

    void IPFSSaver::SaveFile(std::string filename, std::shared_ptr<void> data) {
        std::cout << (char*)data.get() << " -> Inside the IPFSSaver::SaveFile Function";
    }
    inline std::vector<uint8_t> operator""_unhex(const char* c, size_t s) {
        return sgns::common::unhex(std::string_view(c, s)).value();
    }
    void IPFSSaver::SaveASync(std::shared_ptr<boost::asio::io_context> ioc, std::function<void(std::shared_ptr<boost::asio::io_context> ioc)> handle_write, std::string filename, std::shared_ptr<std::vector<char>> data) {
        std::cout << "Inside the IPFSSaver::SaveASync Function" << std::endl;
        if (data == nullptr)
        {
            throw range_error("Can not save with null data");
        }
        sgns::ipfs_lite::rocksdb::Options options;
        options.create_if_missing = true;
        auto r = sgns::ipfs_lite::rocksdb::create("ipfsdb", options);
        auto datastore = sgns::ipfs_lite::ipfs::RocksdbDatastore(r.value());
        //auto datastore = sgns::ipfs_lite::ipfs::InMemoryDatastore();
        gsl::span<const uint8_t> byteSpan(
            reinterpret_cast<const uint8_t*>(data->data()),
            data->size());
        //auto cid = CID::fromBytes(byteSpan);
        auto cid = sgns::common::getCidOf(std::vector<uint8_t>(data->begin(), data->end()));
        common::Buffer buffer(std::vector<uint8_t>(data->begin(), data->end()));
        //sgns::CID cid1{
        //   CID::Version::V1,
        //   libp2p::multi::MulticodecType::Code::SHA2_256,
        //   libp2p::multi::Multihash::create(libp2p::multi::HashType::sha256,
        //                     "0123456789ABCDEF0123456789ABCDEF"_unhex)
        //       .value() };
        datastore.set(cid.value(), buffer);
    }
}

