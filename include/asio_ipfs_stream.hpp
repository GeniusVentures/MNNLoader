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

	template<class Token>
	inline
	typename Result<Token, NodeId>::type
	create(const std::string& repo_path, Token&& token) {
            Handler<Token, NodeId> handler(std::forward<Token>(token));
	    Result<Token, NodeId> result(handler);
	    _create(repo_path, std::move(handler));
	    return result.get();
	};

	// add file 
	template<class Token>
        inline
        typename Result<Token, ContentId>::type
	addFile(const uint8_t* data, Token&& token) {
           Handler<Token, ContentId> handler(std::forward<Token>(token));
	   Result<Token, ContentId> result(handler);
	   add_(data, std::move(handler));
	   return result.get();
	}


	// remove File
	template<class Token>
	inline
        typename Result<Token, outrcome::result<bool>>::type
        removeFile(const NodeId& ) {
            Handler<Token, outrcome::result<bool>> handler(std::forward<Token>(token));
            Result<Token, outrcome::result<bool>> result(handler);
            remove_(node_id, std::move(handler));
            return result.get();
	}

	// download file
	template<class Token>
	inline
        typename Result<Token, FileContent>::type
	getFile(const NodeId& node_id) {
            Handler<Token, FileContent> handler(std::forward<Token>(token));
	    Result<Token, FileContent> result(handler);
	    get_(node_id, std::move(handler));
	    return result.get();
	}


	// add file in the node
	template<class Token>
        typename Result<Token, ContentId>::type
	operator<<(const AsioIPFSStream&, const uint8_t* path); 

	// cat file 
	template<class Token>
        typename Result<Token, FileContent>::type
	operator>>(const AsioIPFSStream&, const ContentID&) const;

    private:
        std::unique_ptr<AsioIPFSIntf> ipfs_intf_;
	NodeId node_id_;
        asio::io_service& ios;	    
	intr::list<HandleBase, intr::constant_time_size<false>> handles;

	// node creation
	void
	_create(const string& repo_path, std::function<void( const boost::system::error_code&, const NodeId&)); 

	void
	_add( const uint8_t* data
             , std::function<void(boost::system::error_code, std::string)>);

	void
	_remove(const NodeId&,
	        std::function<void(boost::system::error_code, std::string)>);

	void
	_get(const NodeId&,
	     std::function<void(boost::system::error_code, std::string)>);
    };
}



#endif // SGNS_ASIO_IPFS_STREAM_HPP
