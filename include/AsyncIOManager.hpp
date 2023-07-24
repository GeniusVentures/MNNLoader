#ifndef SGNS_ASYNC_IOMANAGER_HPP
#define SGNS_ASYNC_IOMANAGER_HPP

#include <string>
#include "IOManager.hpp"
#include "IOStream.hpp"
#include "AsyncIOChannel.hpp"

using namespace sgns;
using namespace io;


namespace sgns::io {

    class AsyncIOManager: public IOManager {

    public:
	using string = std::string;
	using StreamDirection = IOStream::StreamDirection;
	using StreamFlags = IOStream::StreamFlags;

	AsyncIOManager(std::shared_ptr<AsyncIOChannel> channel);
        virtual ~AsyncIOManager() {}

	bool getFile(const std::string&, const std::string&);
	bool putFile(const std::string& fpath, const std::string& dstPath);


    private:
	std::shared_ptr<AsyncIOChannel> async_channel_;
    };
    
}

#endif // SGNS_ASYNC_IOMANAGER_HPP

