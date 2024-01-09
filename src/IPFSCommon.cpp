//IPFSCommon.cpp
#include "IPFSCommon.hpp"
//namespace sgns::ipfspeer {
//    using libp2p::Host;
//    using libp2p::peer::PeerId;
//    using libp2p::peer::PeerInfo;
//
//
//    IPFSHost::IPFSHost() {
//        // Constructor implementation
//    }
//  std::string_view IPFSHost::getLibp2pVersion() const {
//    return "0.0.0";
//  }
//
//  std::string_view IPFSHost::getLibp2pClientVersion() const {
//    return "libp2p";
//  }
//
//  libp2p::peer::PeerId IPFSHost::getId() const {
//    //return idmgr_->getId();
//  }
//
//  libp2p::peer::PeerInfo IPFSHost::getPeerInfo() const {
//    auto addresses = getAddresses();
//    auto observed = getObservedAddresses();
//    auto interfaces = getAddressesInterfaces();
//
//    std::set<libp2p::multi::Multiaddress> unique_addresses;
//    unique_addresses.insert(std::make_move_iterator(addresses.begin()),
//                            std::make_move_iterator(addresses.end()));
//    unique_addresses.insert(std::make_move_iterator(interfaces.begin()),
//                            std::make_move_iterator(interfaces.end()));
//    unique_addresses.insert(std::make_move_iterator(observed.begin()),
//                            std::make_move_iterator(observed.end()));
//
//    // TODO(xDimon): Needs to filter special interfaces (e.g. INADDR_ANY, etc.)
//    for (auto i = unique_addresses.begin(); i != unique_addresses.end();) {
//      bool is_good_addr = true;
//      for (auto &pv : i->getProtocolsWithValues()) {
//        if (pv.first.code == libp2p::multi::Protocol::Code::IP4) {
//          if (pv.second == "0.0.0.0") {
//            is_good_addr = false;
//            break;
//          }
//        } else if (pv.first.code == libp2p::multi::Protocol::Code::IP6) {
//          if (pv.second == "::") {
//            is_good_addr = false;
//            break;
//          }
//        }
//      }
//      if (!is_good_addr) {
//        i = unique_addresses.erase(i);
//      } else {
//        ++i;
//      }
//    }
//
//    std::vector<multi::Multiaddress> unique_addr_list(
//        std::make_move_iterator(unique_addresses.begin()),
//        std::make_move_iterator(unique_addresses.end()));
//
//    return {getId(), std::move(unique_addr_list)};
//  }
//
//  std::vector<libp2p::multi::Multiaddress> IPFSHost::getAddresses() const {
//    return libp2p::network_->getListener().getListenAddresses();
//  }
//
//  std::vector<libp2p::multi::Multiaddress> IPFSHost::getAddressesInterfaces() const {
//    return libp2p::network_->getListener().getListenAddressesInterfaces();
//  }
//
//  std::vector<libp2p::multi::Multiaddress> IPFSHost::getObservedAddresses() const {
//    //auto r = repo_->getAddressRepository().getAddresses(getId());
//    //if (r) {
//    //  return r.value();
//    //}
//
//    // we don't know our addresses
//    return {};
//  }
//
//  Host::Connectedness IPFSHost::connectedness(const libp2p::peer::PeerInfo &p) const {
//    //auto conn = network_->getConnectionManager().getBestConnectionForPeer(p.id);
//    //if (conn != nullptr) {
//    //  return Connectedness::CONNECTED;
//    //}
//
//    //// for each address, try to find transport to dial
//    //for (auto &&ma : p.addresses) {
//    //  if (auto tr = transport_manager_->findBest(ma); tr != nullptr) {
//    //    // we can dial to the peer
//    //    return Connectedness::CAN_CONNECT;
//    //  }
//    //}
//
//    //auto res = repo_->getAddressRepository().getAddresses(p.id);
//    //if (res.has_value()) {
//    //  for (auto &&ma : res.value()) {
//    //    if (auto tr = transport_manager_->findBest(ma); tr != nullptr) {
//    //      // we can dial to the peer
//    //      return Connectedness::CAN_CONNECT;
//    //    }
//    //  }
//    //}
//
//    // we did not find available transports to dial
//    return Connectedness::CAN_NOT_CONNECT;
//  }
//
//  void IPFSHost::setProtocolHandler(
//      const libp2p::peer::Protocol &proto,
//      const std::function<libp2p::connection::Stream::Handler> &handler) {
//    //network_->getListener().getRouter().setProtocolHandler(proto, handler);
//  }
//
//  void IPFSHost::setProtocolHandler(
//      const libp2p::peer::Protocol &proto,
//      const std::function<libp2p::connection::Stream::Handler> &handler,
//      const std::function<bool(const libp2p::peer::Protocol &)> &predicate) {
//    //libp2p::network_->getListener().getRouter().setProtocolHandler(proto, handler,
//    //                                                       predicate);
//  }
//
//  void IPFSHost::newStream(const libp2p::peer::PeerInfo &p,
//                            const libp2p::peer::Protocol &protocol,
//                            const Host::StreamResultHandler &handler,
//                            std::chrono::milliseconds timeout) {
//      libp2p::network_->getDialer().newStream(p, protocol, handler, timeout);
//  }
//
//  void IPFSHost::newStream(const libp2p::peer::PeerId &peer_id,
//                            const libp2p::peer::Protocol &protocol,
//                            const StreamResultHandler &handler) {
//    //network_->getDialer().newStream(peer_id, protocol, handler);
//  }
//
//  libp2p::outcome::result<void> IPFSHost::listen(const libp2p::multi::Multiaddress &ma) {
//    return network_->getListener().listen(ma);
//  }
//
//  libp2p::outcome::result<void> IPFSHost::closeListener(
//      const libp2p::multi::Multiaddress &ma) {
//    //return network_->getListener().closeListener(ma);
//  }
//
//  libp2p::outcome::result<void> IPFSHost::removeListener(
//      const libp2p::multi::Multiaddress &ma) {
//    //return network_->getListener().removeListener(ma);
//  }
//
//  void IPFSHost::start() {
//    //network_->getListener().start();
//  }
//
//  libp2p::event::Handle IPFSHost::setOnNewConnectionHandler(
//      const NewConnectionHandler &h) const {
//    return bus_->getChannel<event::network::OnNewConnectionChannel>().subscribe(
//        [h{std::move(h)}](const std::weak_ptr<connection::CapableConnection>& conn) {
//          auto connection = conn.lock();
//          if (connection) {
//            auto remote_peer_res = connection->remotePeer();
//            if (!remote_peer_res)
//              return;
//
//            auto remote_peer_addr_res = connection->remoteMultiaddr();
//            if (!remote_peer_addr_res)
//              return;
//
//            if (h != nullptr)
//              h(peer::PeerInfo{std::move(remote_peer_res.value()),
//                               std::vector<multi::Multiaddress>{
//                                   std::move(remote_peer_addr_res.value())}});
//          }
//        });
//  }
//
//  void IPFSHost::stop() {
//    network_->getListener().stop();
//  }
//
//  network::Network &IPFSHost::getNetwork() {
//    return *network_;
//  }
//
//  peer::PeerRepository &IPFSHost::getPeerRepository() {
//    return *repo_;
//  }
//
//  network::Router &IPFSHost::getRouter() {
//    return network_->getListener().getRouter();
//  }
//
//  event::Bus &IPFSHost::getBus() {
//    return *bus_;
//  }
//
//  void IPFSHost::connect(const peer::PeerInfo &peer_info,
//                          const ConnectionResultHandler &handler,
//                          std::chrono::milliseconds timeout) {
//    network_->getDialer().dial(peer_info, handler, timeout);
//  }
//
//  void IPFSHost::disconnect(const peer::PeerId &peer_id) {
//    network_->closeConnections(peer_id);
//  }
//    boost::outcome_v2::result<void> IPFSHost::connectTo(
//        const PeerId& peer) {
//        PeerInfo peer_info = host_->getPeerRepository().getPeerInfo(peer);
//        host_->connect(peer_info);
//        return boost::outcome_v2::success();
//    }
//}

