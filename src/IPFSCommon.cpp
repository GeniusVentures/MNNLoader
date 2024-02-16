//IPFSCommon.cpp
#include "IPFSCommon.hpp"


namespace sgns
{
    std::shared_ptr<IPFSDevice> IPFSDevice::instance_;
    std::mutex IPFSDevice::mutex_;

    outcome::result<std::shared_ptr<IPFSDevice>> IPFSDevice::getInstance(std::shared_ptr<boost::asio::io_context> ioc) 
    {
        //Create IPFSDevice if needed
        std::lock_guard<std::mutex> lock(mutex_);

        if (!instance_) {
            instance_ = std::shared_ptr<IPFSDevice>(new IPFSDevice(ioc));
            auto ma = libp2p::multi::Multiaddress::create("/ip4/127.0.0.1/tcp/40000").value();

            //Create DHT
            auto listenresult = instance_->host_->listen(ma);
            if (!listenresult)
            {
                instance_.reset();
                return listenresult.error();
            }
            instance_->bitswap_->start();
            instance_->host_->start();
            instance_->dht_->Start();
        }

        return instance_;
    }

    IPFSDevice::IPFSDevice(std::shared_ptr<boost::asio::io_context> ioc) 
    {
        //Make Kademlia Injector
        libp2p::protocol::kademlia::Config kademlia_config;
        kademlia_config.randomWalk.enabled = true;
        kademlia_config.randomWalk.interval = std::chrono::seconds(300);
        kademlia_config.requestConcurency = 20;
        //auto injector = libp2p::injector::makeHostInjector();
        auto injector = libp2p::injector::makeHostInjector(
            // libp2p::injector::useKeyPair(kp), // Use predefined keypair
            libp2p::injector::makeKademliaInjector(
                libp2p::injector::useKademliaConfig(kademlia_config)));
        host_ = injector.create<std::shared_ptr<libp2p::Host>>();

        // Initialize Bitswap using the created host
        bitswap_ = std::make_shared<sgns::ipfs_bitswap::Bitswap>(*host_, host_->getBus(), ioc);
        //Initialize address holder
        peerAddresses_ = std::make_shared<std::vector<libp2p::peer::PeerInfo>>();

        //Create Kademlia
        auto kademlia =
            injector
            .create<std::shared_ptr<libp2p::protocol::kademlia::Kademlia>>();

        //Initialize DHT
        dht_ = std::make_shared<sgns::IpfsDHT>(kademlia, bootstrapAddresses_);
    }

    bool IPFSDevice::StartFindingPeers(
        std::shared_ptr<boost::asio::io_context> ioc,
        const sgns::ipfs_bitswap::CID& cid,
        std::string filename,
        int addressoffset,
        bool parse,
        bool save,
        CompletionCallback handle_read,
        StatusCallback status
    )
    {
        status(16);
        dht_->FindProviders(cid, [=](libp2p::outcome::result<std::vector<libp2p::peer::PeerInfo>> res) {
            status(17);
            if (!res) {
                std::cerr << "Cannot find providers: " << res.error().message() << std::endl;
                status(-17);
                return false;
            }
            std::cout << "Providers: " << std::endl;
            auto& providers = res.value();
            if (!providers.empty())
            {
                addAddresses(providers);
                //for (auto& provider : providers) {
                //    std::cout << provider.id.toBase58() << std::endl;
                //    auto providerid = provider.id.toBase58();
                    
                    //for (const auto& address : provider.addresses) {
                        
                        // Assuming addAddress function accepts a multiaddress as argument
                        //bool hasPeerId = address.hasProtocol(libp2p::multi::Protocol::Code::P2P);
                        //if (hasPeerId) {
                        //    std::cout << "Address: " << address.getStringAddress() << std::endl;
                        //    addAddress(address);
                        //}
                    //}
                //}

                return RequestBlockMain(ioc, cid, filename, 0, parse, save, handle_read, status);
            }
            else
            {
                std::cout << "Empty providers list received" << std::endl;
                status(-18);
                return false;
                //std::exit(EXIT_FAILURE);
            }
            });
        return false;
    }
    bool IPFSDevice::RequestBlockMain(
        std::shared_ptr<boost::asio::io_context> ioc,
        const sgns::ipfs_bitswap::CID& cid,
        std::string filename,
        int addressoffset,
        bool parse,
        bool save,
        CompletionCallback handle_read,
        StatusCallback status)
    {
        //std::cout << "request main block" << std::endl;
        if (addressoffset < peerAddresses_->size())
        {
            bitswap_->RequestBlock(peerAddresses_->at(addressoffset), cid,
                [=](libp2p::outcome::result<std::string> data)
                {
                    if (data)
                    {
                        auto cidV0 = libp2p::multi::ContentIdentifierCodec::encodeCIDV0(data.value().data(), data.value().size());
                        auto maincid = libp2p::multi::ContentIdentifierCodec::decode(gsl::span((uint8_t*)cidV0.data(), cidV0.size()));

                        //Convert data content into usable span uint8_t
                        gsl::span<const uint8_t> byteSpan(
                            reinterpret_cast<const uint8_t*>(data.value().data()),
                            data.value().size());
                        //Create a PB Decoder to handle the data
                        auto decoder = ipfs_lite::ipld::IPLDNodeDecoderPB();
                        //Attempt to decode
                        auto diddecode = decoder.decode(byteSpan);
                        if (diddecode.has_error())
                        {
                            //Handle Error
                            status(-14);
                            handle_read(ioc, std::make_shared<std::vector<char>>(), false, false);
                            return false;
                        }
                        //std::cout << "ContentTest" << decoder.getContent() << std::endl;
                        status(15);
                        //Start Adding to list
                        CIDInfo cidInfo(maincid.value());
                        for (size_t i = 0; i < decoder.getLinksCount(); ++i) {
                            auto subcid = libp2p::multi::ContentIdentifierCodec::decode(gsl::span((uint8_t*)decoder.getLinkCID(i).data(), decoder.getLinkCID(i).size()));
                            auto scid = libp2p::multi::ContentIdentifierCodec::fromString(libp2p::multi::ContentIdentifierCodec::toString(subcid.value()).value()).value();
                            //If we have a link name, this is a file CID, and not a linked CID for that file
                            //If we don't have a link name, this is a linked CID. This shouldn't happen here in the main request though.
                            if (!decoder.getLinkName(i).empty())
                            {
                                cidInfo.directories.push_back(decoder.getLinkName(i));
                                cidInfo.mainCIDs.push_back(subcid.value());
                            }
                            else
                            {
                                CIDInfo::LinkedCIDInfo linkedCID(subcid.value(), maincid.value(), std::string());
                                std::cout << "add Linked CID: nothing here" << std::endl;
                                cidInfo.linkedCIDs.push_back(linkedCID);
                            }
                            //Increment Outstanding
                            cidInfo.outstandingRequests_++;
                            //Request Additional CID
                            RequestBlockSub(ioc, cid, cid, scid, decoder.getLinkName(i), 0, parse, save, handle_read, status);
                        }

                        //Add to list in IPFSDevice
                        addCID(cidInfo);

                        //If there are no links, this was a single file with 1 block containing all the data, so we can write it out
                        if (decoder.getLinksCount() <= 0)
                        {
                            //Get data, ignoring bytes at beginning or end TODO: need a better way to do this, some contexts the offset is not 6/4.
                            //auto bindata = std::make_shared<std::vector<char>>(decoder.getContent().begin() + 4, decoder.getContent().end() - 2);
                            auto bindata = std::vector<char>(decoder.getContent().begin() + 6, decoder.getContent().end() - 4);
                            requestedCIDs_.end()->finalcontents.first.push_back(filename);
                            requestedCIDs_.end()->finalcontents.second.push_back(bindata);
                            //bool allset = CheckIfAllSet(cid);
                            if (requestedCIDs_.end()->outstandingRequests_ <= 0)
                            {
                                requestedCIDs_.end()->groupLinkedCIDs();
                                requestedCIDs_.end()->writeFinalContentsToDirectories();
                            }
                            //std::cout << "IPFS Finish" << std::endl;
                            //status(0);
                            //handle_read(ioc, bindata, parse, save);
                        }
                        return true;
                    }
                    else
                    {
                        return RequestBlockMain(ioc, cid, filename, addressoffset + 1, parse, save, handle_read, status);
                    }
                });
        }
        return false;
    }

    bool IPFSDevice::RequestBlockSub(
        std::shared_ptr<boost::asio::io_context> ioc,
        const sgns::ipfs_bitswap::CID& cid,
        const sgns::ipfs_bitswap::CID& parentcid,
        const sgns::ipfs_bitswap::CID& scid,
        std::string directory,
        int addressoffset,
        bool parse,
        bool save,
        CompletionCallback handle_read,
        StatusCallback status)
    {
        //std::cout << "Request SubBlock" << std::endl;
        //std::cout << "directory: " << directory << std::endl;
        if (addressoffset < peerAddresses_->size())
        {
            bitswap_->RequestBlock(peerAddresses_->at(addressoffset), scid,
                [=](libp2p::outcome::result<std::string> data)
                {
                    if (data)
                    {
                        //Get CIDInfo Index
                        size_t mainindex = findRequestedCIDIndex(cid);
                        //Decrement 
                        requestedCIDs_[mainindex].outstandingRequests_--;


                        auto cidV0 = libp2p::multi::ContentIdentifierCodec::encodeCIDV0(data.value().data(), data.value().size());
                        auto maincid = libp2p::multi::ContentIdentifierCodec::decode(gsl::span((uint8_t*)cidV0.data(), cidV0.size()));

                        //Convert data content into usable span uint8_t
                        gsl::span<const uint8_t> byteSpan(
                            reinterpret_cast<const uint8_t*>(data.value().data()),
                            data.value().size());

                        //Create a PB Decoder to handle the data
                        auto decoder = ipfs_lite::ipld::IPLDNodeDecoderPB();

                        //Attempt to decode
                        auto diddecode = decoder.decode(byteSpan);
                        if (diddecode.has_error())
                        {
                            //Handle Error
                            status(-15);
                            handle_read(ioc, std::make_shared<std::vector<char>>(), false, false);
                            return false;
                        }
                        for (size_t i = 0; i < decoder.getLinksCount(); ++i) {
                            auto subcid = libp2p::multi::ContentIdentifierCodec::decode(gsl::span((uint8_t*)decoder.getLinkCID(i).data(), decoder.getLinkCID(i).size()));
                            auto sscid = libp2p::multi::ContentIdentifierCodec::fromString(libp2p::multi::ContentIdentifierCodec::toString(subcid.value()).value()).value();
                            std::string newdir = directory + "/" + decoder.getLinkName(i);
                            if (!decoder.getLinkName(i).empty())
                            {
                                requestedCIDs_[mainindex].directories.push_back(newdir);
                                requestedCIDs_[mainindex].mainCIDs.push_back(subcid.value());
                            }
                            else {
                                newdir = directory;
                                CIDInfo::LinkedCIDInfo linkedCID(subcid.value(), scid, newdir);
                                requestedCIDs_[mainindex].linkedCIDs.push_back(linkedCID);
                            }
                            requestedCIDs_[mainindex].outstandingRequests_++;
                            RequestBlockSub(ioc, cid, scid, sscid, newdir, 0, parse, save, handle_read, status);
                        }
                        //If there are no links, this block is complete and we can see if we have all blocks for writing
                        if (decoder.getLinksCount() <= 0)
                        {
                            //Get data, ignoring bytes at beginning or end TODO: need a better way to do this, some contexts the offset is not 6/4.
                            auto bindata = std::vector<char>(decoder.getContent().begin() + 6, decoder.getContent().end() - 4);
                            //Set Content for linked CID, or otherwise push data to final contents if it has none
                            bool setsubdata = setContentForLinkedCID(cid, scid, bindata);
                            if (!setsubdata)
                            {
                                requestedCIDs_[mainindex].finalcontents.first.push_back(directory);
                                requestedCIDs_[mainindex].finalcontents.second.push_back(bindata);
                            }
                            //bool allset = CheckIfAllSet(cid);
                            if (requestedCIDs_[mainindex].outstandingRequests_ <= 0)
                            {
                                requestedCIDs_[mainindex].groupLinkedCIDs();
                                requestedCIDs_[mainindex].writeFinalContentsToDirectories();
                                //std::cout << "IPFS Finish" << std::endl;
                                //status(0);
                                //handle_read(ioc, finaldata, parse, save);
                            }
                        }


                        return true;
                    }
                    else
                    {
                        //Request Block on next address
                        return RequestBlockSub(ioc, cid, parentcid, scid, directory, addressoffset + 1, parse, save, handle_read, status);
                    }
                });
        }
        return false;
    }

    bool IPFSDevice::setContentForLinkedCID(const sgns::ipfs_bitswap::CID& mainCID,
        const sgns::ipfs_bitswap::CID& linkedCID,
        const std::vector<char>& content)
    {
        auto it = std::find_if(requestedCIDs_.begin(), requestedCIDs_.end(),
            [&mainCID](const CIDInfo& info) {
                return info.mainCID == mainCID;
            });

        if (it != requestedCIDs_.end())
        {
            // Update the content for the linked CID within the found CIDInfo
            return it->setContentForLinkedCID(linkedCID, content);
        }
        return false;
    }

    bool IPFSDevice::CheckIfAllSet(const sgns::ipfs_bitswap::CID& mainCID)
    {
        auto it = std::find_if(requestedCIDs_.begin(), requestedCIDs_.end(),
            [&mainCID](const CIDInfo& info) {
                return info.mainCID == mainCID;
            });
        if (it != requestedCIDs_.end())
        {
            return it->allLinkedCIDsHaveContent();
        }
        return false;
    }

    std::shared_ptr<std::vector<char>> IPFSDevice::combineLinkedCIDs(const sgns::ipfs_bitswap::CID& mainCID)
    {
        auto it = std::find_if(requestedCIDs_.begin(), requestedCIDs_.end(),
            [&mainCID](const CIDInfo& info) {
                return info.mainCID == mainCID;
            });
        auto combinedContent = std::make_shared<std::vector<char>>();
        if (it != requestedCIDs_.end())
        {
            // Get the combined content
            combinedContent = it->combineContents();
        }
        return combinedContent;
    }

    size_t IPFSDevice::addCID(CIDInfo& cidInfo)
    {
        // Acquire lock to safely modify the list
        std::lock_guard<std::mutex> lock(mutex_);

        // Add the CIDInfo to the list
        requestedCIDs_.push_back(std::move(cidInfo));
        return requestedCIDs_.size() - 1;
    }

    void IPFSDevice::addAddress(
        libp2p::multi::Multiaddress address
    )
    {
        std::vector<libp2p::multi::Multiaddress> addresses;
        addresses.push_back(address);
        auto peerId = libp2p::peer::PeerId::fromBase58(address.getPeerId().value());
        auto peerInfo = sgns::Peer{
            libp2p::peer::PeerInfo{peerId.value(), std::move(addresses)}
        };
        peerAddresses_->push_back(peerInfo.info);
    }

    void IPFSDevice::addAddresses(const std::vector<libp2p::peer::PeerInfo>& addresses) {
        peerAddresses_->insert(peerAddresses_->end(), addresses.begin(), addresses.end());
    }

    std::shared_ptr<sgns::ipfs_bitswap::Bitswap> IPFSDevice::getBitswap() const {
        return bitswap_;
    }
    std::shared_ptr<libp2p::Host> IPFSDevice::getHost() const {
        return host_;
    }

    IpfsDHT::IpfsDHT(
        std::shared_ptr<libp2p::protocol::kademlia::Kademlia> kademlia,
        std::vector<std::string> bootstrapAddresses)
        : kademlia_(std::move(kademlia))
        , bootstrapAddresses_(bootstrapAddresses)
    {
    }

    void IpfsDHT::Start()
    {
        std::cout << "DHT Start" << std::endl;
        auto&& bootstrapNodes = GetBootstrapNodes();
        for (auto& bootstrap_node : bootstrapNodes) {
            std::cout << "node" << std::endl;
            kademlia_->addPeer(bootstrap_node, true);
        }

        kademlia_->start();
    }

    bool IpfsDHT::FindProviders(
        const libp2p::multi::ContentIdentifier& cid,
        std::function<void(libp2p::outcome::result<std::vector<libp2p::peer::PeerInfo>> onProvidersFound)> onProvidersFound)
    {
        std::cout << "find providers" << std::endl;
        auto kadCID = libp2p::protocol::kademlia::ContentId::fromWire(
            libp2p::multi::ContentIdentifierCodec::encode(cid).value());
        if (!kadCID)
        {
            //logger_->error("Wrong CID {}",
            //    libp2p::peer::PeerId::fromHash(cid.content_address).value().toBase58());
            std::cerr << "Wrong CID" << std::endl;
            return false;
            // TODO: pass an error to callback
            //onProvidersFound(ERROR);
        }
        else
        {
            std::cout << "actually find providers" << std::endl;
            [[maybe_unused]] auto res = kademlia_->findProviders(
                kadCID.value(), 0, onProvidersFound);
            return true;
        }
    }

    std::vector<libp2p::peer::PeerInfo> IpfsDHT::GetBootstrapNodes() const
    {
        std::unordered_map<libp2p::peer::PeerId,
            std::vector<libp2p::multi::Multiaddress>>
            addresses_by_peer_id;

        for (auto& address : bootstrapAddresses_) {
            auto ma = libp2p::multi::Multiaddress::create(address).value();
            auto peer_id_base58 = ma.getPeerId().value();
            auto peer_id = libp2p::peer::PeerId::fromBase58(peer_id_base58).value();

            addresses_by_peer_id[std::move(peer_id)].emplace_back(std::move(ma));
        }

        std::vector<libp2p::peer::PeerInfo> v;
        v.reserve(addresses_by_peer_id.size());
        for (auto& i : addresses_by_peer_id) {
            v.emplace_back(libp2p::peer::PeerInfo{
                /*.id =*/ i.first, /*.addresses =*/ {std::move(i.second)} });
        }

        return v;
    }

    void IpfsDHT::FindPeer(
        const libp2p::peer::PeerId& peerId,
        std::function<void(libp2p::outcome::result<libp2p::peer::PeerInfo>)> onPeerFound)
    {
        [[maybe_unused]] auto res = kademlia_->findPeer(peerId, onPeerFound);
    }
}
