#ifndef SGNS_ASYNC_IOMANAGER_HPP
#define SGNS_ASYNC_IOMANAGER_HPP

#include <string>
#include "include/AsyncIOChannel.hpp"

namespace sgns::io {

    class AsyncIOManager: public IOManager {

    public:
	AsyncIOManager(const AsyncIOChannel& channel);
        virtual ~AsyncIOManager() {}

	std::outcome<void> getFile(const std::string&, const std::string&);
	std::outcome<void> putFile(const std::string& fpath, const std::string& dstPath);


    private:
	AsyncIOChannel async_channel_;
    };
    
}

#endif // SGNS_ASYNC_IOMANAGER_HPP

