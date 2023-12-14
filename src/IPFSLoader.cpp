// IPFSLoader.cpp

#include <FileManager.hpp>
#include <IPFSLoader.hpp>
#include <IPFSCommon.hpp>

const std::string logger_config(R"(
# ----------------
sinks:
  - name: console
    type: console
    color: true
groups:
  - name: main
    sink: console
    level: info
    children:
      - name: libp2p
# ----------------
  )");

SINGLETON_PTR_INIT(IPFSLoader);
static size_t run_time_msec = 0;
size_t ipfsloader::common::Node::requests_sent = 0;
size_t ipfsloader::common::Node::responses_received = 0;
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
static std::shared_ptr<spdlog::logger> logger;

std::shared_ptr<void> IPFSLoader::LoadFile(std::string filename) {
    std::cout << "IPFSLOAD:" << filename << std::endl;

    // prepare log system
    auto logging_system = std::make_shared<soralog::LoggingSystem>(
        std::make_shared<soralog::ConfiguratorFromYAML>(
            // Original LibP2P logging config
            std::make_shared<libp2p::log::Configurator>(),
            // Additional logging config for application
            logger_config));
    auto r = logging_system->configure();
    if (!r.message.empty()) {
        (r.has_error ? std::cerr : std::cout) << r.message << std::endl;
    }
    if (r.has_error) {
        exit(EXIT_FAILURE);
    }

    libp2p::log::setLoggingSystem(logging_system);
    if (std::getenv("TRACE_DEBUG") != nullptr) {
        libp2p::log::setLevelOfGroup("main", soralog::Level::TRACE);
    }
    else {
        libp2p::log::setLevelOfGroup("main", soralog::Level::ERROR_);
    }

    boost::asio::io_context ioc;
    auto io = std::make_shared<boost::asio::io_context>();
    auto work = make_work_guard(io);

    std::vector<std::string> strings({ "xxx", "yyy", "zzz" });
    size_t unexpected = 0;

    auto server_data = std::make_shared<ipfsloader::common::TestDataService>();
    auto client_data = std::make_shared<ipfsloader::common::TestDataService>();
    auto server_cb = [&unexpected](sgns::CID, sgns::common::Buffer) { std::cout << "server data" << std::endl; };
    auto client_cb = [&client_data, &unexpected](sgns::CID cid, sgns::common::Buffer data) {
        std::cout << "data:" << data << std::endl;
    };
    for (const auto& s : strings) {
        // client expects what server has

        server_data->addData(s);
        client_data->addExpected(s);
    }

    ipfsloader::common::Node server(io, server_data, server_cb, 0);
    ipfsloader::common::Node client(io, client_data, client_cb, 3);

    std::shared_ptr<sgns::ipfs_lite::ipfs::IpfsBlockService> block_service;
    sgns::CID test{
        sgns::CID::Version::V1,
        libp2p::multi::MulticodecType::Code::SHA2_256,
        libp2p::multi::Multihash::create(libp2p::multi::HashType::sha256,
            "0123456789ABCDEF0123456789ABCDEF"_unhex).value()
    };
    auto listen_to =
        libp2p::multi::Multiaddress::create("/ip4/127.0.0.1/tcp/40000").value();
    io->post([&]() {
        // server listens
        server.listen(listen_to);
        auto peer = server.getId();
        bool use_address = true;

        // client makes 3 requests

        for (const auto& [cid, _] : client_data->getExpected()) {
            boost::optional<libp2p::multi::Multiaddress> address(listen_to);

            // don't need to pass the address more than once
            client.makeRequest(peer, use_address ? address : boost::none, cid);
            use_address = false;
        }
        });
    ipfsloader::common::runEventLoop(io, run_time_msec);
    client.stop();
    server.stop();

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







