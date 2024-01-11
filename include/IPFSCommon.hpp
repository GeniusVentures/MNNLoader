//IPFSCommon.hpp
#include <iostream>
#include <memory>
#include "logger.hpp"
#include <bitswap.hpp>
#include <boost/asio/io_context.hpp>
#include <libp2p/injector/host_injector.hpp>
#include <libp2p/log/configurator.hpp>
#include <libp2p/protocol/identify/identify.hpp>
#include <libp2p/multi/content_identifier_codec.hpp>
#include <libp2p/protocol/ping/ping.hpp>

#ifndef IPFSCOMMON_HPP
#define IPFSCOMMON_HPP
namespace sgns
{
	class IPFSDevice {
	public:
		//IPFSDevice(std::shared_ptr<boost::asio::io_context> ioc) {
		//	initializeHostAndBitswap(ioc);
		//}
		static std::shared_ptr<IPFSDevice> getInstance(std::shared_ptr<boost::asio::io_context> ioc);
		std::shared_ptr<sgns::ipfs_bitswap::Bitswap> getBitswap() const;
		std::shared_ptr<libp2p::Host> getHost() const;
		~IPFSDevice() {
			// Cleanup resources if needed
		}
		bool RequestBlockMain(
			std::shared_ptr<boost::asio::io_context> ioc,
			const sgns::ipfs_bitswap::CID& cid,
			std::vector<libp2p::multi::Multiaddress>::const_iterator addressBeginIt,
			std::vector<libp2p::multi::Multiaddress>::const_iterator addressEndIt,
			std::function<void(std::shared_ptr<boost::asio::io_context> ioc, std::shared_ptr<std::vector<char>> buffer, bool parse, bool save)> handle_read,
			std::function<void(const int&)> status);
	private:
		//void initializeHostAndBitswap(std::shared_ptr<boost::asio::io_context> ioc);
		IPFSDevice(std::shared_ptr<boost::asio::io_context> ioc);

		static std::shared_ptr<IPFSDevice> instance_;
		static std::mutex mutex_;

		std::shared_ptr<libp2p::Host> host_;
		std::shared_ptr<sgns::ipfs_bitswap::Bitswap> bitswap_;
	};
}

#endif