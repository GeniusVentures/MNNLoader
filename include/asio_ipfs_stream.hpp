#ifndef SGNS_ASIO_IPFS_STREAM_HPP
#define SGNS_ASIO_IPFS_STREAM_HPP

// ASIO IPFS Service converts IPFS Interface 
// calls to asynchronous calls using boost asio service


namespace sgns::asio::ipfs {

    template<class F> struct Defer { F f; ~Defer() { f(); } };
    template<class F> Defer<F> defer(F&& f) { return Defer<F>{forward<F>(f)}; }
    
    struct HandleBase : public intr::list_base_hook
                                <intr::link_mode<intr::auto_unlink>> {
        virtual void cancel() = 0;
        virtual ~HandleBase() { }
    };

    class AsioIPFSStream {
        using Timer = boost::asio::steady_timer;
        using Cancel = std::function<void()>;

        template<class Token, class... Ret>
        using Handler = typename boost::asio::handler_type
                            < Token
                            , void(boost::system::error_code, Ret...)
                            >::type;

        template<class Token, class... Ret>
        using Result = typename boost::asio::async_result<Handler<Token, Ret...>>;

        using string_view = boost::string_view;

	AsioIPFSStream(asio::io_service&);
	AsioIPFSStream(asio::io_service&, NodeId& );

	// add file 
	ContentId& addFile(const uint8_t* data, size_t size, Token&& token) const;
	// remove File
        outrcome::result<bool> removeFile(const NodeId& ) const;
	// download file
	FileContent& getFile(const NodeId& ) const;


	// add file in the node
	ContentId& operator<<(const AsioIPFSStream&, const uint8_t* path) const; 
	// cat file 
	FileContent& operator>>(const AsioIPFSStream&, const ContentID&) const;
    private:
        std::unique_ptr<AsioIPFSIntf> ipfs_intf_;
	NodeId node_id_;
        asio::io_service& ios;	    
	intr::list<HandleBase, intr::constant_time_size<false>> handles;

	// node creation
	NodeId& _create() const; 
    };
}



#endif // SGNS_ASIO_IPFS_STREAM_HPP
