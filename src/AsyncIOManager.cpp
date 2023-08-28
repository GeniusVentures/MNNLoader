
#include "AsyncIOManager.hpp"
#include <fstream>
#include <boost/bind.hpp>


namespace sgns::io {

    AsyncIOManager::AsyncIOManager(std::shared_ptr<AsyncIOChannel> channel)
	    : async_channel_(channel) {

    }

    void AsyncIOManager::registerDevice(const string& schema,
                                        AsyncIODevice *device) {
        async_io_devices_[schema] = device;
    }

    /*
     * Get the file from the device and write to local file  
     */
    bool AsyncIOManager::getFile(const std::string& srcPath, const std::string& fpath) {
	std::shared_ptr<AsyncIODevice> srcDevice = nullptr;
        
	size_t schemaPos = srcPath.find("://");
        if (schemaPos == std::string::npos) {
            throw std::range_error("Src Path is not correct");
        }
        // source device
        auto srcSchema = srcPath.substr(0, schemaPos);
        auto it = async_io_devices_.find(srcSchema);
        if (it == async_io_devices_.end()) {
            throw std::range_error("No Device registered for the src schema");
        }
        srcDevice = std::shared_ptr<AsyncIODevice>(it->second);

	// Open the input device
        // flags if any
        auto srcStream = srcDevice->open(async_channel_->getContext(),srcPath, StreamDirection::READ_ONLY, StreamFlags::NONE);
        if (srcStream == nullptr) {
            throw std::range_error("Src device open failed");
        }
	// check if async is supported
	if(!srcStream->isAsyncSupported()) {
            throw std::range_error("Device does not support async read");
	}

	auto handler = boost::bind(&AsyncIOChannel::onGetComplete, 
		                   async_channel_,
                                   fpath,
		                   boost::asio::placeholders::bytes_transferred, 
		                   boost::asio::placeholders::error); 

	uint32_t buffSize = 1024;
        std::vector<char> buf(buffSize);
        srcStream->readAsync(buf.data(), buffSize, handler);

	return true;
    }
    /*
     * Put the local file to another device
     */
    bool AsyncIOManager::putFile(const std::string& fpath, const std::string& dstPath) {
        std::shared_ptr<AsyncIODevice> dstDevice = nullptr;	

	// destination device
        auto schemaPos = dstPath.find("://");
        if (schemaPos == std::string::npos) {
            throw std::range_error("Dest Path is not correct");
        }
        auto dstSchema = dstPath.substr(0, schemaPos);
        auto it = async_io_devices_.find(dstSchema);
        if (it == async_io_devices_.end()) {
            throw std::range_error("No Device registered for the dst schema");
        }
        dstDevice = std::shared_ptr<AsyncIODevice>(it->second);

	auto flags = StreamFlags::STREAM_APPEND;
	auto dstStream = dstDevice->open(dstPath, StreamDirection::WRITE_ONLY, flags);
        if (dstStream == nullptr) {
            throw std::range_error("Dest device open failed");
        }
	// check if async is supported
	if(!dstStream->isAsyncSupported()) {
            throw std::range_error("Device does not support async write");
	}

        auto handler = boost::bind(&AsyncIOChannel::onPutComplete,
                                   async_channel_,
                                   fpath,
                                   boost::asio::placeholders::bytes_transferred,
                                   boost::asio::placeholders::error);

	// read file
	std::ifstream srcFile(fpath, std::ios::binary);
	if(!srcFile) {
            // error
	}
	srcFile.seekg(0, std::ios::end);
	std::streampos fileSize = srcFile.tellg();
	srcFile.seekg(0, std::ios::beg);
	std::vector<char> buffer(fileSize);
	if (srcFile.read(buffer.data(), fileSize)) {
	    
	}

        dstStream->writeAsync(buffer.data(), fileSize, handler);

	return true;
    }

}
