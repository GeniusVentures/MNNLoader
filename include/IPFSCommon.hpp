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
	/**
	 * A struct holding IPFS CIDs and related content for reconstructing
	 * an entire file grabbed over bitswap
	 */
	struct CIDInfo
	{
		/**
		 * Data, which contains the main CID and any linked CIDs stored as a vector, as well as any associated data
		 * an entire file grabbed over bitswap
		 */
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

		/**
		 * Set the data for a linked CID
		 * @param linkedCID - Linked CID to set content to
		 * @param content - Content to insert
		 */
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

		/**
		 * Check whether we have gotten data for all linked CIDs
		 */
		bool allLinkedCIDsHaveContent() const
		{
			return std::all_of(linkedCIDs.begin(), linkedCIDs.end(),
				[](const LinkedCIDInfo& linkedCIDInfo) {
					return !linkedCIDInfo.content.empty();
				});
		}

		/**
		 * Create a shared buffer vector with data from all linked CIDs combined to create a single file.
		 */
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

	/**
	 * This class creates an IPFS Device and has a function to download
	 * from an IPFS node(s).
	 */
	class IPFSDevice {
	public:
		/**
		 * Completion callback template. We expect an io_context so the thread can be shut down if no outstanding async loads exist, and a buffer with the read information
		 * @param ioc - asio io context so we can stop this if no outstanding async tasks remain
		 * @param buffer - Contains data loaded
		 * @param parse - Whether to parse file upon completion (for MNN)
		 * @param save - Whether to save the file to local disk upon completion
		 */
		using CompletionCallback = std::function<void(std::shared_ptr<boost::asio::io_context> ioc, std::shared_ptr<std::vector<char>> buffer, bool parse, bool save)>;
		/**
		 * Status callback returns an error code as an async load proceeds
		 * @param ioc - asio io context so we can stop this if no outstanding async tasks remain
		 * @param buffer - Contains data loaded
		 * @param parse - Whether to parse file upon completion (for MNN)
		 * @param save - Whether to save the file to local disk upon completion
		 */
		using StatusCallback = std::function<void(const int&)>;

		/**
		 * Create an IPFS Singlelton Device and return instance
		 * @param ioc - Asio io context to use
		 */
		static outcome::result<std::shared_ptr<IPFSDevice>> getInstance(std::shared_ptr<boost::asio::io_context> ioc);
		/**
		 * Get bitswap from device
		 */
		std::shared_ptr<sgns::ipfs_bitswap::Bitswap> getBitswap() const;
		/**
		 * Get host from device
		 */
		std::shared_ptr<libp2p::Host> getHost() const;

		~IPFSDevice() {
			// Cleanup resources if needed
		}

		/**
		 * Add the Main CID for a file to bitswap wantlist to get information or file(if small enough)
		 * @param ioc - Asio io context to use
		 * @param cid - IPFS Main CID to get from bitswap
		 * @param addressoffset - Offset from list of addresses to use, usually want to call 0 on this as it will loop through from starting point if needed
		 * @param parse - Whether to parse file upon completion (for MNN currently)
		 * @param save - Whether to save the file to local disk upon completion
		 * @param handle_read - Filemanager callback on completion
		 * @param status - Status function that will be updated with status codes as operation progresses
		 */
		bool RequestBlockMain(
			std::shared_ptr<boost::asio::io_context> ioc,
			const sgns::ipfs_bitswap::CID& cid,
			int addressoffset,
			bool parse,
			bool save,
			CompletionCallback handle_read,
			StatusCallback status);

		/**
		 * Add an address to pool of addresses to try to get file using IPFS bitswap
		 * @param address - libp2p multiaddress to add to pool /ip4/127.0.0.1/tcp/4001/p2p/CID format
		 */
		void addAddress(
			libp2p::multi::Multiaddress address
		);

		/**
		 * Add the data of a linked CID to list containing all linked CIDs and associated data
		 * @param mainCID - Main CID of file we are getting
		 * @param linkedCID - Linked CID to main CID to get a portion of file
		 * @param content - The content we retrieved for the linked CID from bitswap
		 */
		bool setContentForLinkedCID(const sgns::ipfs_bitswap::CID& mainCID,
			const sgns::ipfs_bitswap::CID& linkedCID,
			const std::vector<char>& content);

		/**
		 * Get a buffer of the combineed data of all linked CIDs to create an entire file.
		 * @param mainCID - Main CID of file we are getting
		 */
		std::shared_ptr<std::vector<char>> combineLinkedCIDs(const sgns::ipfs_bitswap::CID& mainCID);

		/**
		 * Add a CIDInfo object to list of CIDs we are trying to get.
		 * @param cidInfo - A CIDInfo object struct as defined above
		 */
		void addCID(const CIDInfo& cidInfo);
	private:
		/**
		 * Create an IPFSDevice along with associated bitswap and host on an asio io_context
		 * @param ioc - Asio io context to use
		 */
		IPFSDevice(std::shared_ptr<boost::asio::io_context> ioc);

		/**
		 * Add the sub CID for a file to bitswap wantlist to get part of file
		 * @param ioc - Asio io context to use
		 * @param cid - IPFS Main CID to get from bitswap
		 * @param scid - Linked CID to get file of
		 * @param addressoffset - Offset from list of addresses to use, usually want to call 0 on this as it will loop through from starting point if needed
		 * @param parse - Whether to parse file upon completion (for MNN currently)
		 * @param save - Whether to save the file to local disk upon completion
		 * @param handle_read - Filemanager callback on completion
		 * @param status - Status function that will be updated with status codes as operation progresses
		 */
		bool RequestBlockSub(
			std::shared_ptr<boost::asio::io_context> ioc,
			const sgns::ipfs_bitswap::CID& cid,
			const sgns::ipfs_bitswap::CID& scid,
			int addressoffset,
			bool parse,
			bool save,
			CompletionCallback handle_read,
			StatusCallback status);

		//Common vars used for getting file from IPFS
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