//IPFSCommon.cpp
#include "IPFSCommon.hpp"
#include "codec/cbor/cbor.hpp"
#include "primitives/big_int.hpp"
#include "ipfs_lite/ipfs/merkledag/merkledag_service.hpp"
#include "ipfs_lite/ipfs/merkledag/leaf.hpp"
#include "ipfs_lite/ipld/impl/ipld_node_decoder_pb.hpp"
#include "ipfs_lite/ipld/impl/ipld_node_encoder_pb.hpp"

namespace sgns
{
    std::shared_ptr<IPFSDevice> IPFSDevice::instance_;
    std::mutex IPFSDevice::mutex_;

    std::shared_ptr<IPFSDevice> IPFSDevice::getInstance(std::shared_ptr<boost::asio::io_context> ioc) {
        //Create IPFSDevice if needed
        std::lock_guard<std::mutex> lock(mutex_);

        if (!instance_) {
            instance_ = std::shared_ptr<IPFSDevice>(new IPFSDevice(ioc));
        }

        return instance_;
    }

    IPFSDevice::IPFSDevice(std::shared_ptr<boost::asio::io_context> ioc) {
        // Use libp2p::injector::makeHostInjector()to create the host
        auto injector = libp2p::injector::makeHostInjector();
        host_ = injector.create<std::shared_ptr<libp2p::Host>>();

        // Initialize Bitswap using the created host
        bitswap_ = std::make_shared<sgns::ipfs_bitswap::Bitswap>(*host_, host_->getBus(), ioc);
    }

    bool IPFSDevice::RequestBlockMain(
        std::shared_ptr<boost::asio::io_context> ioc,
        const sgns::ipfs_bitswap::CID& cid,
        std::vector<libp2p::multi::Multiaddress>::const_iterator addressBeginIt,
        std::vector<libp2p::multi::Multiaddress>::const_iterator addressEndIt,
        std::function<void(std::shared_ptr<boost::asio::io_context> ioc, std::shared_ptr<std::vector<char>> buffer, bool parse, bool save)> handle_read,
        std::function<void(const int&)> status)
    {
        if (addressBeginIt != addressEndIt)
        {
            auto peerId = libp2p::peer::PeerId::fromBase58(addressBeginIt->getPeerId().value()).value();
            auto address = *addressBeginIt;
            bitswap_->RequestBlock({ peerId, { address } }, cid,
                [=](libp2p::outcome::result<std::string> data)
                {
                    if (data)
                    {
                        //std::cout << "Bitswap data received: " << data.value() << std::endl;
                        //auto cidV0 = libp2p::multi::ContentIdentifierCodec::encodeCIDV0(data.value().data(), data.value().size());
                        //auto cid = libp2p::multi::ContentIdentifierCodec::decode(gsl::span((uint8_t*)cidV0.data(), cidV0.size()));
                        gsl::span<const uint8_t> byteSpan(
                            reinterpret_cast<const uint8_t*>(data.value().data()),
                            data.value().size());
                        auto decoder = ipfs_lite::ipld::IPLDNodeDecoderPB();
                        
                        auto diddecode = decoder.decode(byteSpan);
                        std::cout << "ContentTest" << decoder.getContent() << std::endl;
                        
                        for (size_t i = 0; i < decoder.getLinksCount(); ++i) {
                            auto subcid = libp2p::multi::ContentIdentifierCodec::decode(gsl::span((uint8_t*)decoder.getLinkCID(i).data(), decoder.getLinkCID(i).size()));
                            auto scid = libp2p::multi::ContentIdentifierCodec::toString(subcid.value()).value();
                            std::cout << "tostring? " << scid << std::endl;
                        }

                        auto bindata = std::make_shared<std::vector<char>>(decoder.getContent().begin(), decoder.getContent().end());
                        handle_read(ioc, bindata, false, true);
                        return true;
                    }
                    else
                    {
                        std::cout << "not data" << std::endl;
                        return RequestBlockMain(ioc, cid, addressBeginIt + 1, addressEndIt, handle_read, status);
                    }
                });
        }
        //status(-14);
        //handle_read(ioc, std::make_shared<std::vector<char>>(), false, false);
        return false;
    }
    std::shared_ptr<sgns::ipfs_bitswap::Bitswap> IPFSDevice::getBitswap() const {
        return bitswap_;
    }
    std::shared_ptr<libp2p::Host> IPFSDevice::getHost() const {
        return host_;
    }
}
