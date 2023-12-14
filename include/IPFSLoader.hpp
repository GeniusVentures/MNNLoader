// IPFSLoader.hpp

#ifndef IPFSLOADER_HPP
#define IPFSLOADER_HPP

#include "FileLoader.hpp"
#include "Singleton.hpp"
#include "ipfs_lite/ipfs/impl/ipfs_block_service.hpp"
#include "libp2p/multi/multihash.hpp"
#include "ipfs_lite/ipfs/impl/datastore_rocksdb.hpp"
#include "common/hexutil.hpp"
#include "common/blob.hpp"
#include <boost/asio.hpp>
#include <libp2p/host/host.hpp>
#include <ipfs_lite/ipfs/graphsync/graphsync.hpp>
#include "common/logger.hpp"
#include <libp2p/log/configurator.hpp>

/// @brief class to handle "ipfs://" prefix in a filename to load from ipfs
class IPFSLoader : public FileLoader {
    SINGLETON_PTR(IPFSLoader);
public:
    /// @brief Load a file from somewhere based on prefix
    /// @param filename URL prefix encoded filename, i.e. https://filename.html, ipfs://filename.mnn
    /// @return a void pointer to the data associated with loading the file into memory
    virtual std::shared_ptr<void> LoadFile(std::string filename) override;
    virtual std::shared_ptr<void> LoadASync(std::string filename, bool parse) override;

};

namespace sgns::ipfs_lite::ipfs::graphsync::test {

    /// runs event loop for max_milliseconds or until SIGINT or SIGTERM
    void runEventLoop(const std::shared_ptr<boost::asio::io_context>& io,
        size_t max_milliseconds);

    // Creates per-node objects using libp2p hos injector
    std::pair<std::shared_ptr<Graphsync>, std::shared_ptr<libp2p::Host>>
        createNodeObjects(std::shared_ptr<boost::asio::io_context> io);

    inline std::ostream& operator << (std::ostream& os, const CID& cid) {
        os << cid.toString().value();
        return os;
    }

    // MerkleDAG bridge interface for test purposes
    class TestDataService : public MerkleDagBridge {
    public:
        using Storage = std::map<CID, common::Buffer>;

        TestDataService& addData(const std::string& s) {
            insertNode(data_, s);
            return *this;
        }

        TestDataService& addExpected(const std::string& s) {
            insertNode(expected_, s);
            return *this;
        }

        const Storage& getData() const {
            return data_;
        }

        const Storage& getExpected() const {
            return expected_;
        }

        const Storage& getReceived() const {
            return received_;
        }

        // places into data_, returns true if expected
        bool onDataBlock(CID cid, common::Buffer data);

    private:
        static void insertNode(Storage& dst, const std::string& data_str);

        outcome::result<size_t> select(
            const CID& cid,
            gsl::span<const uint8_t> selector,
            std::function<bool(const CID& cid, const common::Buffer& data)> handler)
            const override;

        Storage data_;
        Storage expected_;
        Storage received_;
    };
    // Test node aggregate
    class Node {
    public:
        // total requests sent by all nodes in a test case
        static size_t requests_sent;

        // total responses received by all nodes in a test case
        static size_t responses_received;

        // n_responses_expected: count of responses received by the node after which
        // io->stop() is called
        Node(std::shared_ptr<boost::asio::io_context> io,
            std::shared_ptr<MerkleDagBridge> data_service,
            Graphsync::BlockCallback cb,
            size_t n_responses_expected)
            : io_(std::move(io)),
            data_service_(std::move(data_service)),
            block_cb_(std::move(cb)),
            n_responses_expected_(n_responses_expected) {
            std::tie(graphsync_, host_) = createNodeObjects(io_);
        }

        // stops graphsync and host, otherwise they can interact with further tests!
        void stop() {
            graphsync_->stop();
            host_->stop();
        }

        // returns peer ID, so they can connect to each other
        auto getId() const {
            return host_->getId();
        }

        // listens to network and starts nodes if not yet started
        void listen(const libp2p::multi::Multiaddress& listen_to) {
            auto listen_res = host_->listen(listen_to);
            if (!listen_res) {
                std::cout << "cannot listen" << std::endl;
                //logger->trace("Cannot listen to multiaddress {}, {}",
                //    listen_to.getStringAddress(),
                //    listen_res.error().message());
                return;
            }
            start();
        }

        // calls Graphsync's makeRequest
        void makeRequest(const libp2p::peer::PeerId& peer,
            boost::optional<libp2p::multi::Multiaddress> address,
            const CID& root_cid) {
            start();

            std::vector<Extension> extensions;
            ResponseMetadata response_metadata{};
            Extension response_metadata_extension =
                encodeResponseMetadata(response_metadata);
            extensions.push_back(response_metadata_extension);
            std::vector<CID> cids;
            Extension do_not_send_cids_extension = encodeDontSendCids(cids);
            extensions.push_back(do_not_send_cids_extension);
            // unused code , request_ is deleted because Subscription have deleted copy-constructor and operator
                  // requests_.push_back(graphsync_->makeRequest(peer,
                  //                                             std::move(address),
                  //                                             root_cid,
                  //                                             {},
                  //                                             extensions,
                  //                                             requestProgressCallback()));
                  // Subscription subscription = graphsync_->makeRequest(peer,
                  //                                             std::move(address),
                  //                                             root_cid,
                  //                                             {},
                  //                                             extensions,
                  //                                             requestProgressCallback());
            requests_.push_back(std::shared_ptr<Subscription>(new Subscription(std::move(graphsync_->makeRequest(peer,
                std::move(address),
                root_cid,
                {},
                extensions,
                requestProgressCallback())))));

            //-------------------------------------------------------------------------------------

            ++requests_sent;
        }

    private:
        void start() {
            if (!started_) {
                graphsync_->start(data_service_, block_cb_);
                host_->start();
                started_ = true;
            }
        }

        // helper, returns requesu callback fn
        Graphsync::RequestProgressCallback requestProgressCallback() {
            static auto formatExtensions =
                [](const std::vector<Extension>& extensions) -> std::string {
                std::string s;
                for (const auto& item : extensions) {
                    s += fmt::format(
                        "({}: 0x{}) ", item.name, common::Buffer(item.data).toHex());
                }
                return s;
            };
            return [this](ResponseStatusCode code,
                const std::vector<Extension>& extensions) {
                    ++responses_received;
                    std::cout << "request progress" << std::endl;
                    //logger->trace("request progress: code={}, extensions={}",
                    //    statusCodeToString(code),
                    //    formatExtensions(extensions));
                    if (++n_responses == n_responses_expected_) {
                        io_->stop();
                    }
            };
        }

        // asion context to be stopped when needed
        std::shared_ptr<boost::asio::io_context> io_;

        std::shared_ptr<Graphsync> graphsync_;

        std::shared_ptr<libp2p::Host> host_;

        std::shared_ptr<MerkleDagBridge> data_service_;

        Graphsync::BlockCallback block_cb_;

        // keeping subscriptions alive, otherwise they cancel themselves
        // class Subscription have non-copyable constructor and operator, so it can not be used in std::vector
        // std::vector<Subscription> requests_;

        std::vector<std::shared_ptr<Subscription >> requests_;

        size_t n_responses_expected_;
        size_t n_responses = 0;
        bool started_ = false;
    };

    //size_t Node::requests_sent = 0;
    //size_t Node::responses_received = 0;

    // Context for more complex cases
    struct NodeParams {
        // listen address
        boost::optional<libp2p::multi::Multiaddress> listen_to;

        // MerkleDAG stub for node
        std::shared_ptr<TestDataService> data_service;

        // Strings to make blocks and CIDs from them
        std::vector<std::string> strings;

        // peer ID
        boost::optional<libp2p::peer::PeerId> peer;
    };
}  // namespace sgns::ipfs_lite::ipfs::graphsync::test
#endif
