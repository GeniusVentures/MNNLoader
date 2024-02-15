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
#include "libp2p/protocol/kademlia/kademlia.hpp"
#include "libp2p/injector/kademlia_injector.hpp"
//TEMP REmove
#include <fstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#ifndef IPFSCOMMON_HPP
#define IPFSCOMMON_HPP
namespace sgns
{
	/**
	 * A struct holding IPFS CIDs and related content for reconstructing
	 * an entire file grabbed over bitswap
	 */

	struct CIDInfo {

		struct Content {
			libp2p::multi::ContentIdentifier cid;
			std::string name;
			bool isDirectory;
			std::vector<char> data;  
			//std::vector<CIDInfo> subDirectories;  
			std::vector<CIDInfo> contents;

			Content(const libp2p::multi::ContentIdentifier& cid, const std::string& name)
				: cid(cid), name(name), isDirectory(true), data(), contents() {}

			//size_t addSubDirectory(CIDInfo subDir) {
			//	subDirectories.push_back(subDir);
			//	return subDirectories.size() - 1;
			//}
			size_t addLink(const CIDInfo link) {
				contents.push_back(link);
				return contents.size() - 1;
			}
			void setData(const std::vector<char> newData) {
				data = newData;
			}
			Content& findContent(const std::vector<size_t> indexRefs, size_t index = 0) {
				if (index >= indexRefs.size()) {
					throw std::out_of_range("Invalid indexRefs");
				}
				//std::cout << "content test index" << indexRefs[index] << std::endl;
				//std::cout << "content indexrefs size: " << indexRefs.size() << std::endl;
				//std::cout << "content contents size: " << contents.size() << std::endl;
				std::cout << index << " @";
				for (auto& ref : indexRefs) {
					std::cout << ref << " ";
				}
				std::cout << std::endl;
				size_t nextIndex = indexRefs[index];
				if (nextIndex >= contents.size()) {
					std::cout << "OrInCrashinhere?" << std::endl;
					throw std::out_of_range("Index out of range");
				}

				if (index == indexRefs.size() - 1) {
					std::cout << "Crashinhere?" << std::endl;
					throw std::out_of_range("Index out of range, last gotten was not a content");
				}
				else {
					return contents[nextIndex].findContent(indexRefs, index + 1);
				}
			}
		};

		libp2p::multi::ContentIdentifier mainCID;
		std::vector<Content> contents;
		std::vector<libp2p::multi::ContentIdentifier> requestedCIDs;

		CIDInfo(const libp2p::multi::ContentIdentifier& cid)
			: mainCID(cid), contents(), requestedCIDs() {}


		Content& findContent(const std::vector<size_t> indexRefs, size_t index = 0) {
			if (index >= indexRefs.size()) {
				throw std::out_of_range("Invalid indexRefs");
			}
			std::cout << index << " @";
			for (auto& ref : indexRefs) {
				std::cout << ref << " ";
			}
			
			std::cout << std::endl;
			//std::cout << "test index" << indexRefs[index] << std::endl;
			//std::cout << "indexrefs size: " << indexRefs.size() << std::endl;
			//std::cout << "contents size: " << contents.size() << std::endl;
			size_t nextIndex = indexRefs[index];
			if (nextIndex >= contents.size()) {
				throw std::out_of_range("Index out of range");
			}

			if (index == indexRefs.size() - 1) {
				return contents[nextIndex];
			}
			else {
				return contents[nextIndex].findContent(indexRefs, index + 1);
			}
		}

		size_t addContent(const libp2p::multi::ContentIdentifier cid, const std::string name) {
			contents.emplace_back(cid, name); 
			return contents.size() - 1;
		}
		//std::shared_ptr<Content> addContent(const libp2p::multi::ContentIdentifier& cid, const std::string& name) {
		//	contents.emplace_back(cid, name);
		//	return std::shared_ptr<Content>(&contents.back()); 
		//}

		void setDirectoryStatus(const libp2p::multi::ContentIdentifier cid, bool isDirectory)
		{
			//Set is directory status.
			auto it = std::find_if(contents.begin(), contents.end(), [&cid](const Content& content) {
				return content.cid == cid;
				});

			if (it != contents.end()) {
				it->isDirectory = isDirectory;
			}
		}

		//bool allContentsHaveData() {
		//	return std::all_of(contents.begin(), contents.end(), [](const Content& content) {
		//		return !content.isDirectory || !content.subDirectories.empty() || !content.data.empty();
		//		});
		//}

		void addRequestedCID(const libp2p::multi::ContentIdentifier cid) {
			requestedCIDs.push_back(cid);
		}

		void removeRequestedCID(const libp2p::multi::ContentIdentifier cid) {
			requestedCIDs.erase(std::remove(requestedCIDs.begin(), requestedCIDs.end(), cid), requestedCIDs.end());
		}

		bool isAllDataReceived() const {
			std::cout << "Remaining:" << requestedCIDs.size() << std::endl;
			return requestedCIDs.empty();
		}

		void combineContents() {
			for (auto& content : contents) {
				if (!content.isDirectory) {
					// If it's not a directory, combine the data from links
					for (const auto& link : content.contents) {
						for (const auto& innercontent : link.contents)
						{
							content.data.insert(content.data.end(), innercontent.data.begin(), innercontent.data.end());
						}
					}
				}
				else {
					// If it's a directory, recursively combine the contents of subdirectories
					for (auto& subDir : content.contents) {
						subDir.combineContents();
					}
				}
			}
		}
		void writeContentsToFile(const std::string directoryPath) {
			std::filesystem::create_directory(directoryPath);
			for (auto& content : contents) {
				std::cout << "NameCHeck: " << content.name << std::endl;
				if (content.isDirectory) {
					// Create a directory
					std::string subDirectoryPath = directoryPath + "/" + content.name;
					std::filesystem::create_directory(subDirectoryPath);

					// Recursively write contents of subdirectories
					for (auto& subDir : content.contents) {
						subDir.writeContentsToFile(subDirectoryPath);
					}
				}
				else {
					// Write file data
					std::cout << "WriteFile: " << content.name << std::endl;
					std::string filePath = directoryPath + "/" + content.name;
					std::ofstream outputFile(filePath, std::ios::binary);
					if (outputFile.is_open()) {
						outputFile.write(content.data.data(), content.data.size());
						outputFile.close();
					}
					else {
						// Open Err
					}
				}
			}
		}
	};

	struct Peer {
		libp2p::peer::PeerInfo info;
	};
	/**
	 * This class creates an DHT for finding peers with CIDs we want
	 * from IPFS node(s).
	 */
	class IpfsDHT
	{
	public:
		IpfsDHT(
			std::shared_ptr<libp2p::protocol::kademlia::Kademlia> kademlia,
			std::vector<std::string> bootstrapAddresses);

		void Start();

		bool FindProviders(
			const libp2p::multi::ContentIdentifier& cid,
			std::function<void(libp2p::outcome::result<std::vector<libp2p::peer::PeerInfo>> onProvidersFound)> onProvidersFound);

		void FindPeer(
			const libp2p::peer::PeerId& peerId,
			std::function<void(libp2p::outcome::result<libp2p::peer::PeerInfo>)> onPeerFound);
	private:
		std::vector<libp2p::peer::PeerInfo> GetBootstrapNodes() const;

		std::shared_ptr<libp2p::protocol::kademlia::Kademlia> kademlia_;
		std::vector<std::string> bootstrapAddresses_;
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

		bool StartFindingPeers(
			std::shared_ptr<boost::asio::io_context> ioc,
			const sgns::ipfs_bitswap::CID& cid,
			int addressoffset,
			bool parse,
			bool save,
			CompletionCallback handle_read,
			StatusCallback status
		);

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
		void addAddresses(const std::vector<libp2p::peer::PeerInfo>& addresses);

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
		size_t addCID(CIDInfo& cidInfo);
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
			size_t maincidIndex,
			const sgns::ipfs_bitswap::CID& parentcid,
			const sgns::ipfs_bitswap::CID& scid,
			std::vector<size_t> cidinfoRef,
			size_t cidInfoContentInd,
			int addressoffset,
			bool parse,
			bool save,
			CompletionCallback handle_read,
			StatusCallback status);


		//Common vars used for getting file from IPFS
		static std::shared_ptr<IPFSDevice> instance_;
		static std::mutex mutex_;

		std::shared_ptr<sgns::IpfsDHT> dht_;
		std::shared_ptr<libp2p::Host> host_;
		std::shared_ptr<sgns::ipfs_bitswap::Bitswap> bitswap_;
		//std::shared_ptr<std::vector<libp2p::multi::Multiaddress>> peerAddresses_;
		std::shared_ptr<std::vector<libp2p::peer::PeerInfo>> peerAddresses_;

		// Maintain a list of requested CIDs along with their linked CIDs and the content of the linked CIDs
		std::vector<CIDInfo> requestedCIDs_;

		//Default Bootstrap Servers
		std::vector<std::string> bootstrapAddresses_ = {
			//"/dnsaddr/bootstrap.libp2p.io/ipfs/QmNnooDu7bfjPFoTZYxMNLWUQJyrVwtbZg5gBMjTezGAJN",
			//"/dnsaddr/bootstrap.libp2p.io/ipfs/QmQCU2EcMqAqQPR2i9bChDtGNJchTbq5TbXJJ16u19uLTa",
			//"/dnsaddr/bootstrap.libp2p.io/ipfs/QmbLHAnMoJPWSCR5Zhtx6BHJX9KiKNN6tpvbUcqanj75Nb",
			//"/dnsaddr/bootstrap.libp2p.io/ipfs/QmcZf59bWwK5XFi76CZX8cbJ4BhTzzA3gU1ZjYZcYW3dwt",
			"/ip4/104.131.131.82/tcp/4001/ipfs/QmaCpDMGvV2BGHeYERUEnRQAwe3N8SzbUtfsmvsqQLuvuJ",            // mars.i.ipfs.io
			"/ip4/104.236.179.241/tcp/4001/ipfs/QmSoLPppuBtQSGwKDZT2M73ULpjvfd3aZ6ha4oFGL1KrGM",           // pluto.i.ipfs.io
			"/ip4/128.199.219.111/tcp/4001/ipfs/QmSoLSafTMBsPKadTEgaXctDQVcqN88CNLHXMkTNwMKPnu",           // saturn.i.ipfs.io
			"/ip4/104.236.76.40/tcp/4001/ipfs/QmSoLV4Bbm51jM9C4gDYZQ9Cy3U6aXMJDAbzgu2fzaDs64",             // venus.i.ipfs.io
			"/ip4/178.62.158.247/tcp/4001/ipfs/QmSoLer265NRgSp2LA3dPaeykiS1J6DifTC88f5uVQKNAd",            // earth.i.ipfs.io
			"/ip6/2604:a880:1:20::203:d001/tcp/4001/ipfs/QmSoLPppuBtQSGwKDZT2M73ULpjvfd3aZ6ha4oFGL1KrGM",  // pluto.i.ipfs.io
			"/ip6/2400:6180:0:d0::151:6001/tcp/4001/ipfs/QmSoLSafTMBsPKadTEgaXctDQVcqN88CNLHXMkTNwMKPnu",  // saturn.i.ipfs.io
			"/ip6/2604:a880:800:10::4a:5001/tcp/4001/ipfs/QmSoLV4Bbm51jM9C4gDYZQ9Cy3U6aXMJDAbzgu2fzaDs64", // venus.i.ipfs.io
			"/ip6/2a03:b0c0:0:1010::23:1001/tcp/4001/ipfs/QmSoLer265NRgSp2LA3dPaeykiS1J6DifTC88f5uVQKNAd", // earth.i.ipfs.io
		};
	};


}

#endif