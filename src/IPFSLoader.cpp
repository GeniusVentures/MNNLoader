#include <sstream>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <string>
#include "FileManager.hpp"
#include "IPFSLoader.hpp"
#include "URLStringUtil.h"
#include "logger.hpp"
#include <bitswap.hpp>
#include <boost/asio/io_context.hpp>
#include <libp2p/injector/host_injector.hpp>
#include <libp2p/log/configurator.hpp>
#include <libp2p/protocol/identify/identify.hpp>
#include <libp2p/multi/content_identifier_codec.hpp>
#include <libp2p/protocol/ping/ping.hpp>



namespace sgns
{
    using libp2p::Host;
    //using sgns::ipfspeer;
    IPFSLoader* IPFSLoader::_instance = nullptr;
    void IPFSLoader::InitializeSingleton() {
        if (_instance == nullptr) {
            _instance = new IPFSLoader();
        }
    }
    IPFSLoader::IPFSLoader()
    {
        FileManager::GetInstance().RegisterLoader("ipfs", this);
    }

    std::shared_ptr<void> IPFSLoader::LoadFile(std::string filename)
    {
        std::shared_ptr<string> result = std::make_shared < string>("init");
        return result;
    }
    std::shared_ptr<libp2p::protocol::PingClientSession> pingSession_;

    void OnSessionPing(libp2p::outcome::result<std::shared_ptr<libp2p::protocol::PingClientSession>> session)
    {
        if (session)
        {
            pingSession_ = std::move(session.value());
        }
    }

    void OnNewConnection(
        const std::weak_ptr<libp2p::connection::CapableConnection>& conn,
        std::shared_ptr<libp2p::protocol::Ping> ping) {
        if (conn.expired()) {
            return;
        }
        auto sconn = conn.lock();
        ping->startPinging(sconn, &OnSessionPing);
    }

    const std::string logger_config(R"(
    # ----------------
    sinks:
      - name: console
        type: console
        color: false
    groups:
      - name: main
        sink: console
        level: error
        children:
          - name: libp2p
    # ----------------
      )");

    std::shared_ptr<void> IPFSLoader::LoadASync(std::string filename, bool parse, bool save, std::shared_ptr<boost::asio::io_context> ioc, CompletionCallback handle_read, StatusCallback status)
    {
        auto logging_system = std::make_shared<soralog::LoggingSystem>(
            std::make_shared<soralog::ConfiguratorFromYAML>(
                // Original LibP2P logging config
                std::make_shared<libp2p::log::Configurator>(),
                // Additional logging config for application
                logger_config));
        auto r = logging_system->configure();
        libp2p::log::setLoggingSystem(logging_system);

        auto loggerProcessingEngine = sgns::ipfs_bitswap::createLogger("Bitswap");
        loggerProcessingEngine->set_level(spdlog::level::debug);
        std::shared_ptr<string> result = std::make_shared < string>("init");

        //Get CID and Filename
        std::string ipfs_cid;
        std::string ipfs_file;
        parseIPFSUrl(filename, ipfs_cid, ipfs_file);
        //std::cout << "IPFS Parse" << ipfs_cid << std::endl;
        //std::cout << "IPFS Parse" << ipfs_file << std::endl;
        //Create Host
        auto ipfsDeviceResult = IPFSDevice::getInstance(ioc);
        if (!ipfsDeviceResult)
        {   
            //Error Listening
            status(-16);
            std::cerr << "Cannot listen address " << ". Error: " << ipfsDeviceResult.error().message() << std::endl;
            handle_read(ioc, std::make_shared<std::vector<char>>(), false, false);
            return result;
        }
        auto ipfsDevice = ipfsDeviceResult.value();
        //auto ma = libp2p::multi::Multiaddress::create("/ip4/127.0.0.1/tcp/40000").value();

        ipfsDevice->addAddress(libp2p::multi::Multiaddress::create("/ip4/127.0.0.1/tcp/4001/p2p/12D3KooWJvtKnbpvdzAUvfkH1TZG5S33DdrnCMWEqb41ut8pdAu9").value());

        //CID of File
        auto cid = libp2p::multi::ContentIdentifierCodec::fromString(ipfs_cid).value();
        
        status(13);
        ioc->post([=] {
            status(14);
            ipfsDevice->RequestBlockMain(ioc, cid, 0, parse, save, handle_read, status);
            });
        
        return result;
    }

} // End namespace sgns
