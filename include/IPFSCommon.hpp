//IPFSCommon.hpp
#include <iostream>
#include <memory>
#include <libp2p/connection/stream.hpp>
#include <libp2p/host/host.hpp>
#include <libp2p/peer/protocol.hpp>
#include "boost/outcome/outcome.hpp"
#include "libp2p/multi/multiaddress.hpp"
#include <libp2p/network/network.hpp>

#ifndef IPFSCOMMON_HPP
#define IPFSCOMMON_HPP
//namespace sgns::ipfspeer {
//    using libp2p::Host;
//    using libp2p::peer::PeerId;
//    //Normally in IPFS all nodes are peers with respect to one another, but here we call it a host. In repsect to ipfs this is a peer, but we use it to connect to other peers.
//    class IPFSHost : public libp2p::Host {
//        // Implement required functions...
//    public:
//        ~IPFSHost() override = default;
//        IPFSHost();
//        std::string_view getLibp2pVersion() const override;
//
//        std::string_view getLibp2pClientVersion() const override;
//
//        libp2p::peer::PeerId getId() const override;
//
//        libp2p::peer::PeerInfo getPeerInfo() const override;
//
//        std::vector<libp2p::multi::Multiaddress> getAddresses() const override;
//
//        std::vector<libp2p::multi::Multiaddress> getAddressesInterfaces() const override;
//
//        std::vector<libp2p::multi::Multiaddress> getObservedAddresses() const override;
//
//        Connectedness connectedness(const libp2p::peer::PeerInfo& p) const override;
//
//        void connect(const libp2p::peer::PeerInfo& peer_info,
//            const ConnectionResultHandler& handler,
//            std::chrono::milliseconds timeout) override;
//
//        void disconnect(const libp2p::peer::PeerId& peer_id) override;
//
//        void setProtocolHandler(
//            const libp2p::peer::Protocol& proto,
//            const std::function<libp2p::connection::Stream::Handler>& handler) override;
//
//        void setProtocolHandler(
//            const libp2p::peer::Protocol& proto,
//            const std::function<libp2p::connection::Stream::Handler>& handler,
//            const std::function<bool(const libp2p::peer::Protocol&)>& predicate) override;
//
//        void newStream(const libp2p::peer::PeerInfo& p, const libp2p::peer::Protocol& protocol,
//            const StreamResultHandler& handler,
//            std::chrono::milliseconds timeout) override;
//
//        void newStream(const libp2p::peer::PeerId& peer_id, const libp2p::peer::Protocol& protocol,
//            const StreamResultHandler& handler) override;
//
//        libp2p::outcome::result<void> listen(const libp2p::multi::Multiaddress& ma) override;
//
//        libp2p::outcome::result<void> closeListener(const libp2p::multi::Multiaddress& ma) override;
//
//        libp2p::outcome::result<void> removeListener(
//            const libp2p::multi::Multiaddress& ma) override;
//
//        void start() override;
//
//        void stop() override;
//
//        libp2p::network::Network& getNetwork() override;
//
//        libp2p::peer::PeerRepository& getPeerRepository() override;
//
//        libp2p::network::Router& getRouter() override;
//
//        libp2p::event::Bus& getBus() override;
//
//        libp2p::event::Handle setOnNewConnectionHandler(
//            const NewConnectionHandler& h) const override;
//
//        boost::outcome_v2::result<void> connectTo(const PeerId& peer);
//
//    private:
//        std::shared_ptr<libp2p::Host> host_;
//    };
//}


#endif