#ifndef SGNS_ASYNC_IOMANAGER_HPP
#define SGNS_ASYNC_IOMANAGER_HPP

#include <string>
#include "IOManager.hpp"
#include "IOStream.hpp"
#include "AsyncIOChannel.hpp"
#include "AsyncIODevice.hpp"

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

	void registerDevice(const string& schema, AsyncIODevice *device);
	bool getFile(const std::string&, const std::string&);
	bool putFile(const std::string& fpath, const std::string& dstPath);


    private:
	std::map<string, AsyncIODevice *> async_io_devices_;
	std::shared_ptr<AsyncIOChannel>   async_channel_;
    };
    
}

#endif // SGNS_ASYNC_IOMANAGER_HPP

