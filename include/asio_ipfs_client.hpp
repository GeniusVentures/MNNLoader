#ifndef SGNS_ASIO_IPFS_CLIENT_HPP
#define SGNS_ASIO_IPFS_CLIENT_HPP

// include the ipfs_lite_cpp header

#include "primitives/cid/cid.hpp"


namespace sgns::asio::ipfs {

    using ContentId = sgns::CID:
    using PeerId = libp2p::peer::PeerId;
    using FileContent = sgns::async::FileContent;
    using NodeId = libp2p::peer::PeerId;
    
    class AsioIPFSClient {

    public:
        AsioIPFSClient(asio::io_service&) 
	virtual ~AsioIPFSClient() override = default;

	// currently ipfslite supports only Add Node & Add file
	// add node/nodes
        NodeId& addNode(void* cb, void* cbArgs) const;	
	// add file to the node
	ContentId& addFile(const std::string &path, void* cb, void* cbArgs) const;
	// retrieves and downloads the content 
	FileContent& getFile(const ContentId &content_id, void* cb, void* cbArgs) const;
	outcome::result<bool> removeNode(const NodeId&, void* cb, void* cbArgs) const;


	// Not supported in ipfslite
	// returns the content 
	inline FileContent& catFile(const ContentId &content_id) const {
            static FileContent fileContent;
	    return fileContent;
	}
	// ipfs pin
	inline outcome::result<bool> pinFile(const ContentId &content_id) const {
	    return false;
	}
	inline outcome::result<bool> unpinFile(const ContentId &content_id) const {
	    return false;
	}
	// list files in a directory
	inline std::list<ContentId> listFiles(const ContentId &content_id) const {
            return std::list<ContentId>();
	}
	// list the peers
	inline std::list<PeerId> peerNodes() const {
            return std::list<PeerId>();
	}
	inline outcome::result<bool> publish(const ContentId &content_id) const {
            return false;
	}
	// resolve
	inline ContentId& resolve(const std::string &name) const e
            static ContentId contentId;
	    return contentId;
	}

    private:
        asio::io_service& io_service_;

     
    }; 

} // namespace sgns::async::


#endif // SGNS_ASYNC_IPFS_INTF_HPP
