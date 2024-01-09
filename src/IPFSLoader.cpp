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
    SINGLETON_PTR_INIT(IPFSLoader);
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

    bool RequestBlock(
        std::shared_ptr<boost::asio::io_context> ioc,
        std::shared_ptr<sgns::ipfs_bitswap::Bitswap> bitswap,
        const sgns::ipfs_bitswap::CID& cid,
        std::vector<libp2p::multi::Multiaddress>::const_iterator addressBeginIt,
        std::vector<libp2p::multi::Multiaddress>::const_iterator addressEndIt)
    {
        std::cout << "Requestblock" << std::endl;
        if (addressBeginIt != addressEndIt)
        {
            auto peerId = libp2p::peer::PeerId::fromBase58(addressBeginIt->getPeerId().value()).value();
            auto address = *addressBeginIt;
            bitswap->RequestBlock({ peerId, { address } }, cid,
                [=](libp2p::outcome::result<std::string> data)
                {
                    if (data)
                    {
                        std::cout << "Bitswap data received: " << data.value() << std::endl;
                        return true;
                    }
                    else
                    {
                        return RequestBlock(ioc, bitswap, cid, addressBeginIt + 1, addressEndIt);
                    }
                });
        }

        return false;
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
        level: debug
        children:
          - name: libp2p
    # ----------------
      )");

    std::shared_ptr<void> IPFSLoader::LoadASync(std::string filename, bool parse, bool save, std::shared_ptr<boost::asio::io_context> ioc, CompletionCallback handle_read, std::function<void(const int&)> status)
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

        //Get CID and Filename
        std::string ipfs_cid;
        std::string ipfs_file;
        parseIPFSUrl(filename, ipfs_cid, ipfs_file);
        std::cout << "IPFS Parse" << ipfs_cid << std::endl;
        std::cout << "IPFS Parse" << ipfs_file << std::endl;
        //Create Host
        auto injector = libp2p::injector::makeHostInjector();
        auto host = injector.create<std::shared_ptr<libp2p::Host>>();

        auto ma = libp2p::multi::Multiaddress::create("/ip4/127.0.0.1/tcp/40000").value();
        auto self_id = host->getId();
        std::cerr << self_id.toBase58() << " * started" << std::endl;

        // Identify protocol initialization
        //auto identityManager = injector.create<std::shared_ptr<libp2p::peer::IdentityManager>>();
        //auto keyMarshaller = injector.create<std::shared_ptr<libp2p::crypto::marshaller::KeyMarshaller>>();

        //auto identifyMessageProcessor = std::make_shared<libp2p::protocol::IdentifyMessageProcessor>(
        //    *host, host->getNetwork().getConnectionManager(), *identityManager, keyMarshaller);
        //auto identify = std::make_shared<libp2p::protocol::Identify>(*host, identifyMessageProcessor, host->getBus());

        std::vector< libp2p::multi::Multiaddress> peerAddresses = {
            //libp2p::multi::Multiaddress::create(
            //    "/ip4/54.89.112.218/tcp/4001/p2p/QmSrq3jnqGAja4z96Jq9SMQFJ8TzbRAgrMLi1sTR6Ane6W").value(),

            libp2p::multi::Multiaddress::create(
                "/ip4/127.0.0.1/tcp/4001/p2p/12D3KooWJvtKnbpvdzAUvfkH1TZG5S33DdrnCMWEqb41ut8pdAu9").value(),

        };

        //CID of File
        auto cid = libp2p::multi::ContentIdentifierCodec::fromString(ipfs_cid).value();

        // Ping protocol setup
        //libp2p::protocol::PingConfig pingConfig{};
        //auto rng = std::make_shared<libp2p::crypto::random::BoostRandomGenerator>();
        //auto ping = std::make_shared<libp2p::protocol::Ping>(*host, host->getBus(), *ioc, rng, pingConfig);

        //auto subsOnNewConnection = host->getBus().getChannel<libp2p::event::network::OnNewConnectionChannel>().subscribe(
        //    [ping](auto&& conn) {
        //        return OnNewConnection(conn, ping);
        //    });

        //host->setProtocolHandler(
        //    ping->getProtocolId(),
        //    [ping](libp2p::protocol::BaseProtocol::StreamResult rstream) {
        //        ping->handle(std::move(rstream));
        //    });

        //Bitswap setup
        auto bitswap = std::make_shared<sgns::ipfs_bitswap::Bitswap>(*host, host->getBus(), ioc);

        //ioc->post([ioc, ma, host, identify, bitswap, cid, peerAddresses] {
        ioc->post([=] {
            auto listen = host->listen(ma);
            if (!listen)
            {
                std::cerr << "Cannot listen address " << ma.getStringAddress().data()
                    << ". Error: " << listen.error().message() << std::endl;
                std::exit(EXIT_FAILURE);
            }

            //identify->start();
            bitswap->start();
            host->start();
            RequestBlock(ioc, bitswap, cid, peerAddresses.begin(), peerAddresses.end());           
            });
        //context->run()


        std::cout << "ok" << std::endl;
        std::shared_ptr<string> result = std::make_shared < string>("init");
        return result;
    }

} // End namespace sgns
