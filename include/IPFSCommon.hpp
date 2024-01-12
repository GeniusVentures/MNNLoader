//IPFSCommon.hpp
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include "logger.hpp"
#include "bitswap.hpp"
#include "boost/asio/io_context.hpp"
#include "libp2p/injector/host_injector.hpp"
#include "libp2p/log/configurator.hpp"
#include "libp2p/protocol/identify/identify.hpp"
#include "libp2p/multi/content_identifier_codec.hpp"
#include "libp2p/protocol/ping/ping.hpp"
#include "ipfs_lite/ipld/impl/ipld_node_decoder_pb.hpp"


#ifndef IPFSCOMMON_HPP
#define IPFSCOMMON_HPP
namespace sgns
{
	// Definition of CIDInfo
	struct CIDInfo
	{
		libp2p::multi::ContentIdentifier mainCID;
		struct LinkedCIDInfo
		{
			libp2p::multi::ContentIdentifier linkedCID;
			std::vector<char> content;
			LinkedCIDInfo(const libp2p::multi::ContentIdentifier& cid)
				: linkedCID(cid), content() {}
		};
		std::vector<LinkedCIDInfo> linkedCIDs;

		CIDInfo(const libp2p::multi::ContentIdentifier& cid)
			: mainCID(cid), linkedCIDs() {}

		// Need to be able to set the content for a linked CID as we get it
		void setContentForLinkedCID(const libp2p::multi::ContentIdentifier& linkedCID, const std::vector<char>& content)
		{
			auto it = std::find_if(linkedCIDs.begin(), linkedCIDs.end(),
				[&linkedCID](const LinkedCIDInfo& info) {
					return info.linkedCID == linkedCID;
				});

			if (it != linkedCIDs.end())
			{
				// Update the content for the linked CID
				it->content = content;
			}
		}

		// Check if all linkedCIDs have content
		bool allLinkedCIDsHaveContent() const
		{
			return std::all_of(linkedCIDs.begin(), linkedCIDs.end(),
				[](const LinkedCIDInfo& linkedCIDInfo) {
					return !linkedCIDInfo.content.empty();
				});
		}

		//Combine Blocks into single set of data
		std::shared_ptr<std::vector<char>> combineContents() const
		{
			auto combinedContent = std::make_shared<std::vector<char>>();

			// Iterate through each linkedCID and appends
			for (const auto& linkedCIDInfo : linkedCIDs)
			{
				combinedContent->insert(combinedContent->end(),
					linkedCIDInfo.content.begin(),
					linkedCIDInfo.content.end());
			}

			return combinedContent;
		}
	};

	class IPFSDevice {
	public:
		//IPFSDevice(std::shared_ptr<boost::asio::io_context> ioc) {
		//	initializeHostAndBitswap(ioc);
		//}
		static outcome::result<std::shared_ptr<IPFSDevice>> getInstance(std::shared_ptr<boost::asio::io_context> ioc);
		std::shared_ptr<sgns::ipfs_bitswap::Bitswap> getBitswap() const;
		std::shared_ptr<libp2p::Host> getHost() const;
		~IPFSDevice() {
			// Cleanup resources if needed
		}

		//Request Main CID Block
		bool RequestBlockMain(
			std::shared_ptr<boost::asio::io_context> ioc,
			const sgns::ipfs_bitswap::CID& cid,
			int addressoffset,
			bool parse,
			bool save,
			std::function<void(std::shared_ptr<boost::asio::io_context> ioc, std::shared_ptr<std::vector<char>> buffer, bool parse, bool save)> handle_read,
			std::function<void(const int&)> status);

		//Request Linked CID blocks
		bool RequestBlockSub(
			std::shared_ptr<boost::asio::io_context> ioc,
			const sgns::ipfs_bitswap::CID& cid,
			const sgns::ipfs_bitswap::CID& scid,
			int addressoffset,
			bool parse,
			bool save,
			std::function<void(std::shared_ptr<boost::asio::io_context> ioc, std::shared_ptr<std::vector<char>> buffer, bool parse, bool save)> handle_read,
			std::function<void(const int&)> status);

		//Add Address to pool
		void addAddress(
			libp2p::multi::Multiaddress address
		);

		//Set content for a linked CID
		bool setContentForLinkedCID(const sgns::ipfs_bitswap::CID& mainCID,
			const sgns::ipfs_bitswap::CID& linkedCID,
			const std::vector<char>& content);

		//Get combined data for all linked CIDs
		std::shared_ptr<std::vector<char>> combineLinkedCIDs(const sgns::ipfs_bitswap::CID& mainCID);

		// Add a CIDinfo block
		void addCID(const CIDInfo& cidInfo);
	private:
		//void initializeHostAndBitswap(std::shared_ptr<boost::asio::io_context> ioc);
		IPFSDevice(std::shared_ptr<boost::asio::io_context> ioc);

		static std::shared_ptr<IPFSDevice> instance_;
		static std::mutex mutex_;

		std::shared_ptr<libp2p::Host> host_;
		std::shared_ptr<sgns::ipfs_bitswap::Bitswap> bitswap_;
		std::shared_ptr<std::vector<libp2p::multi::Multiaddress>> peerAddresses_;

		// Maintain a list of requested CIDs along with their linked CIDs and the content of the linked CIDs
		std::vector<CIDInfo> requestedCIDs_;
	};


}

#endif