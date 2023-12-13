// IPFSLoader.cpp

#include "FileManager.hpp"
#include "IPFSLoader.hpp"
#include <boost/di/extension/scopes/shared.hpp>
#include <libp2p/injector/host_injector.hpp>
#include <libp2p/protocol/common/asio/asio_scheduler.hpp>
#include <ipfs_lite/ipfs/graphsync/impl/graphsync_impl.hpp>
#include <ipfs_lite/ipld/impl/ipld_node_impl.hpp>
#include <codec/cbor/cbor.hpp>
#include <boost/optional/optional_io.hpp>

SINGLETON_PTR_INIT(IPFSLoader);
static size_t run_time_msec = 0;

IPFSLoader::IPFSLoader() {
    FileManager::GetInstance().RegisterLoader("ipfs", this);
}
//inline auto operator""_cid(const char* c, size_t s) {
//    return sgns::CID::fromBytes(sgns::common::unhex(std::string_view(c, s)).value())
//        .value();
//}
//inline std::vector<uint8_t> operator""_unhex(const char* c, size_t s) {
//    return sgns::common::unhex(std::string_view(c, s)).value();
//}
std::shared_ptr<void> IPFSLoader::LoadFile(std::string filename) {
    std::cout << "IPFSLOAD:" << filename << std::endl;
    //boost::asio::io_context ioc;
    //auto io = std::make_shared<boost::asio::io_context>();
    //auto work = make_work_guard(io);

    //std::vector<std::string> strings({ "xxx", "yyy", "zzz" });
    //size_t unexpected = 0;

    //auto server_data = std::make_shared<sgns::ipfs_lite::ipfs::graphsync::test::TestDataService>();
    //auto client_data = std::make_shared<sgns::ipfs_lite::ipfs::graphsync::test::TestDataService>();
    //auto server_cb = [&unexpected](sgns::CID, sgns::common::Buffer) { std::cout << "server data" << std::endl; };
    //auto client_cb = [&client_data, &unexpected](sgns::CID cid, sgns::common::Buffer data) {
    //    std::cout << "data" << std::endl;
    //};
    //for (const auto& s : strings) {
    //    // client expects what server has

    //    server_data->addData(s);
    //    client_data->addExpected(s);
    //}

    //sgns::ipfs_lite::ipfs::graphsync::test::Node server(io, server_data, server_cb, 0);
    //sgns::ipfs_lite::ipfs::graphsync::test::Node client(io, client_data, client_cb, 3);

    //std::shared_ptr<sgns::ipfs_lite::ipfs::IpfsBlockService> block_service;
    //sgns::CID test{
    //    sgns::CID::Version::V1,
    //    libp2p::multi::MulticodecType::Code::SHA2_256,
    //    libp2p::multi::Multihash::create(libp2p::multi::HashType::sha256,
    //        "0123456789ABCDEF0123456789ABCDEF"_unhex).value()
    //};
    //auto listen_to =
    //    libp2p::multi::Multiaddress::create("/ip4/127.0.0.1/tcp/40000").value();
    //io->post([&]() {
    //    // server listens
    //    server.listen(listen_to);
    //    auto peer = server.getId();
    //    bool use_address = true;

    //    // client makes 3 requests

    //    for (const auto& [cid, _] : client_data->getExpected()) {
    //        boost::optional<libp2p::multi::Multiaddress> address(listen_to);

    //        // don't need to pass the address more than once
    //        client.makeRequest(peer, use_address ? address : boost::none, cid);
    //        use_address = false;
    //    }
    //    });
    //sgns::ipfs_lite::ipfs::graphsync::test::runEventLoop(io, run_time_msec);
    //client.stop();
    //server.stop();

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








//namespace sgns::ipfs_lite::ipfs::graphsync::test {
//    void runEventLoop(const std::shared_ptr<boost::asio::io_context>& io,
//        size_t max_milliseconds) {
//        boost::asio::signal_set signals(*io, SIGINT, SIGTERM);
//
//        // io->run() can exit if we're not waiting for anything
//        signals.async_wait(
//            [&io](const boost::system::error_code&, int) { io->stop(); });
//
//        if (max_milliseconds > 0) {
//            io->run_for(std::chrono::milliseconds(max_milliseconds));
//        }
//        else {
//            io->run();
//        }
//    }
//
//    std::pair<std::shared_ptr<Graphsync>, std::shared_ptr<libp2p::Host>>
//        createNodeObjects(std::shared_ptr<boost::asio::io_context> io) {
//
//        // [boost::di::override] allows for creating multiple hosts for testing
//        // purposes
//        auto injector =
//            libp2p::injector::makeHostInjector<boost::di::extension::shared_config>(
//                boost::di::bind<boost::asio::io_context>.to(
//                    io)[boost::di::override]);
//
//        std::pair<std::shared_ptr<Graphsync>, std::shared_ptr<libp2p::Host>>
//            objects;
//        objects.second = injector.template create<std::shared_ptr<libp2p::Host>>();
//        auto scheduler = std::make_shared<libp2p::protocol::AsioScheduler>(
//            io, libp2p::protocol::SchedulerConfig{});
//        objects.first =
//            std::make_shared<GraphsyncImpl>(objects.second, std::move(scheduler));
//        return objects;
//    }
//
//    bool TestDataService::onDataBlock(CID cid, common::Buffer data) {
//        bool expected = false;
//        auto it = expected_.find(cid);
//        if (it != expected_.end() && it->second == data) {
//            expected = (received_.count(cid) == 0);
//        }
//        received_[cid] = std::move(data);
//        return expected;
//    }
//
//    void TestDataService::insertNode(TestDataService::Storage& dst,
//        const std::string& data_str) {
//        using NodeImpl = sgns::ipfs_lite::ipld::IPLDNodeImpl;
//        auto node = NodeImpl::createFromString(data_str);
//        dst[node->getCID()] = node->getRawBytes();
//    }
//
//    outcome::result<size_t> TestDataService::select(
//        const CID& cid,
//        gsl::span<const uint8_t> selector,
//        std::function<bool(const CID& cid, const common::Buffer& data)> handler)
//        const {
//        auto it = data_.find(cid);
//        if (it != data_.end()) {
//            handler(it->first, it->second);
//            return 1;
//        }
//        return 0;
//    }
//}  // namespace sgns::ipfs_lite::ipfs::graphsync::test