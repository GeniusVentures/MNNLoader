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
        std::shared_ptr<sgns::ipfs_bitswap::Bitswap> bitswap,
        const sgns::ipfs_bitswap::CID& cid,
        std::vector<libp2p::multi::Multiaddress>::iterator addressBeginIt,
        std::vector<libp2p::multi::Multiaddress>::iterator addressEndIt)
    {
        if (addressBeginIt != addressEndIt)
        {
            auto peerId = libp2p::peer::PeerId::fromBase58(addressBeginIt->getPeerId().value()).value();
            auto address = *addressBeginIt;
            bitswap->RequestBlock({ peerId, { address } }, cid,
                [bitswap, cid, addressBeginIt, addressEndIt](libp2p::outcome::result<std::string> data)
                {
                    if (data)
                    {
                        std::cout << "Bitswap data received: " << data.value() << std::endl;
                        return true;
                    }
                    else
                    {
                        return RequestBlock(bitswap, cid, addressBeginIt + 1, addressEndIt);
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
        //std::cout << "IPFS Parse" << ipfs_cid << std::endl;
        //std::cout << "IPFS Parse" << ipfs_file << std::endl;
        //Create Host
        auto injector = libp2p::injector::makeHostInjector();
        auto host = injector.create<std::shared_ptr<libp2p::Host>>();
        auto context = injector.create<std::shared_ptr<boost::asio::io_context>>();
        auto ma = libp2p::multi::Multiaddress::create("/ip4/127.0.0.1/tcp/40000").value();
        auto self_id = host->getId();
        std::cerr << self_id.toBase58() << " * started" << std::endl;

        // Identify protocol initialization
        auto identityManager = injector.create<std::shared_ptr<libp2p::peer::IdentityManager>>();
        auto keyMarshaller = injector.create<std::shared_ptr<libp2p::crypto::marshaller::KeyMarshaller>>();

        auto identifyMessageProcessor = std::make_shared<libp2p::protocol::IdentifyMessageProcessor>(
            *host, host->getNetwork().getConnectionManager(), *identityManager, keyMarshaller);
        auto identify = std::make_shared<libp2p::protocol::Identify>(*host, identifyMessageProcessor, host->getBus());

        std::vector< libp2p::multi::Multiaddress> peerAddresses = {
            //libp2p::multi::Multiaddress::create(
            //    "/ip4/54.89.112.218/tcp/4001/p2p/QmSrq3jnqGAja4z96Jq9SMQFJ8TzbRAgrMLi1sTR6Ane6W").value(),

            libp2p::multi::Multiaddress::create(
                "/ip4/127.0.0.1/tcp/4001/p2p/12D3KooWJvtKnbpvdzAUvfkH1TZG5S33DdrnCMWEqb41ut8pdAu9").value(),

        };

        //CID of File
        auto cid = libp2p::multi::ContentIdentifierCodec::fromString(ipfs_cid).value();

        // Ping protocol setup
        libp2p::protocol::PingConfig pingConfig{};
        auto rng = std::make_shared<libp2p::crypto::random::BoostRandomGenerator>();
        auto ping = std::make_shared<libp2p::protocol::Ping>(*host, host->getBus(), *context, rng, pingConfig);

        auto subsOnNewConnection = host->getBus().getChannel<libp2p::event::network::OnNewConnectionChannel>().subscribe(
            [ping](auto&& conn) {
                return OnNewConnection(conn, ping);
            });

        host->setProtocolHandler(
            ping->getProtocolId(),
            [ping](libp2p::protocol::BaseProtocol::StreamResult rstream) {
                ping->handle(std::move(rstream));
            });

        //Bitswap setup
        auto bitswap = std::make_shared<sgns::ipfs_bitswap::Bitswap>(*host, host->getBus(), context);

        context->post([&] {
            auto listen = host->listen(ma);
            if (!listen)
            {
                std::cerr << "Cannot listen address " << ma.getStringAddress().data()
                    << ". Error: " << listen.error().message() << std::endl;
                std::exit(EXIT_FAILURE);
            }

            identify->start();
            bitswap->start();
            host->start();

            RequestBlock(bitswap, cid, peerAddresses.begin(), peerAddresses.end());
            });

        boost::asio::signal_set signals(*context, SIGINT, SIGTERM);
        signals.async_wait(
            [&context](const boost::system::error_code&, int) { context->stop(); });
        context->run();
        //context->post([host{ std::move(host) }] {
        //    std::string ipfsPeerIdString = "12D3KooWJvtKnbpvdzAUvfkH1TZG5S33DdrnCMWEqb41ut8pdAu9";


        //    auto address = libp2p::multi::Multiaddress::create("/ip4/127.0.0.1/tcp/4001/p2p/12D3KooWJvtKnbpvdzAUvfkH1TZG5S33DdrnCMWEqb41ut8pdAu9");
        //    if (!address) {
        //        std::cerr << "Failed to create Multiaddress: " << address.error().message() << std::endl;
        //    }
        //    auto server_ma = std::move(address.value());
        //    auto server_peer_id_str = server_ma.getPeerId();


        //    auto peerid_res = libp2p::peer::PeerId::fromBase58(*server_peer_id_str);
        //    auto server_peer_id = std::move(peerid_res.value());
        //    auto peer_info = libp2p::peer::PeerInfo{ server_peer_id, {server_ma}};
        //    
        //    auto listenadd = libp2p::multi::Multiaddress::create("/ip4/127.0.0.1/tcp/7001/p2p/" + host->getPeerInfo().id.toBase58());
        //    auto listen_ma = std::move(listenadd.value());

        //    libp2p::peer::Protocol ipfsProtocol = "/ipfs/bitswap/1.2.0";
        //    
        //    std::cout << "Host:" << host->getPeerInfo().id.toBase58() << std::endl;
        //    gsl::span<const libp2p::multi::Multiaddress> ma_span{ &server_ma, 1 };
        //    
        //    host->start();
        //    host->getPeerRepository().getAddressRepository().addAddresses(server_peer_id, ma_span, std::chrono::milliseconds(0));
        //    host->listen(listen_ma);
        //    std::cout << "Add:" << host->getAddresses().data() << std::endl;
        //    auto run_duration = std::chrono::seconds(5000);
        //    host->newStream(peer_info, ipfsProtocol, [host, peer_info, ipfsProtocol, server_ma, server_peer_id](auto&& stream_res) {
        //        std::cout << "test" << std::endl;
        //            if (!stream_res)
        //            {
        //                std::cout << "Stream:" << stream_res.error().message() << std::endl;
        //                
        //            }
        //            //std::cout << "Stream?" stream_res.message() << std::endl;
        //            std::cout << " connected?" << std::endl;
        //            auto stream = std::move(stream_res.value());
        //            //auto desiredBlockCID = CID::fromString("QmX6XijWPwDijdiRBmUAhuk2AAY8PSHbFwmg1VF7gzXCAW");
        //            std::string requestData = "want QmX6XijWPwDijdiRBmUAhuk2AAY8PSHbFwmg1VF7gzXCAW" ;
        //            gsl::span<const uint8_t> requestDataSpan(reinterpret_cast<const uint8_t*>(requestData.data()), requestData.size());
        //            stream->write(requestDataSpan, requestData.size(), [stream, host, peer_info, ipfsProtocol, server_ma, server_peer_id](auto&& write_res) {
        //                if (write_res.has_error()) {
        //                    // Handle write error
        //                    std::cerr << "Error writing to stream: " << write_res.error().message() << std::endl;
        //                }
        //                std::cout << "inhere" << std::endl;
        //                std::vector<uint8_t> buffer(1024);  // Your buffer to store the read data

        //            //    // Create a gsl::span from the std::vector<uint8_t> buffer
        //                gsl::span<uint8_t> bufferSpan(buffer);


        //                stream->read(bufferSpan, buffer.size(), [stream, host, peer_info, ipfsProtocol, server_ma, server_peer_id, buffer, bufferSpan](auto&& read_res) {
        //                    if (read_res.has_error()) {
        //                        // Handle read error
        //                        std::cerr << "Error reading from stream: " << read_res.error().message() << std::endl;
        //                        return;
        //                    }
        //                    std::cout << "inread" << std::endl;
        //                    });
        //                });
        //        });
        //    });
        //context->run();



        std::cout << "ok" << std::endl;
        std::shared_ptr<string> result = std::make_shared < string>("init");
        return result;
    }

} // End namespace sgns
