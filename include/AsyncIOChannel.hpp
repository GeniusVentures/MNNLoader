#ifndef SGNS_ASYNC_IOCHANNEL_HPP
#define SGNS_ASYNC_IOCHANNEL_HPP

#include <boost/asio.hpp>

/*
 * ASYNC IO Communication channel
 * Event completion handlers 
 */


namespace sgns::io {
    
    class AsyncIOChannel {
    public:
	using string = std::string;
        using AsioContext = boost::asio::io_context;

	AsyncIOChannel(const AsioContext& context);
	virtual void onGetComplete(const string& path,
			           std::size_t bytes_transfered,
			           const boost::system::error_code& error) = 0;
	virtual void onPutComplete(const string& path,
			           std::size_t bytes_transfered,
			           const boost::system::error_code& error) = 0;
	virtual void onMoveComplete(const string& path,
			           std::size_t bytes_transfered,
			           const boost::system::error_code& error) = 0;

			            
	inline std::shared_ptr<AsioContext> getContext() {
	    return context_;
	}

    private:
	std::shared_ptr<AsioContext> context_;

    };
}


#endif // SGNS_ASYNC_IONOTIFIER_HPP
