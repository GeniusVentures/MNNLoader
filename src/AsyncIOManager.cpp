
#include "AsyncIOManager.hpp"


namespace sgns::io {

    AsyncIOManager::AsyncIOManager(const AsyncIOChannel& channel)
	    : async_channel_(channel) {

    }

    /*
     * Get the file from the device and write to local file  
     */
    std::outcome<void> IOManager::getFile(const std::string& srcPath, const std::string& fpath) {
	std::shared_ptr<IODevice> srcDevice = nullptr;
        
	size_t schemaPos = srcPath.find("://");
        if (schemaPos == std::string::npos) {
            throw std::range_error("Src Path is not correct");
        }
        // source device
        auto srcSchema = srcPath.substr(0, schemaPos);
        auto it = io_devices_.find(srcSchema);
        if (it == io_devices_.end()) {
            throw std::range_error("No Device registered for the src schema");
        }
        srcDevice = it->second();

	// Open the input device
        // flags if any
        auto srcStream = srcDevice->open(srcPath, StreamMode::READ_ONLY);
        if (srcStream == nullptr) {
            throw std::range_error("Src device open failed");
        }
	// check if async is supported
	if(!srcStream->isAsyncSupported()) {
            throw std::range_error("Device does not support async read");
	}

	auto handler = boost::bind(&async_channel_.onGetComplete, 
		                   this,
                                   fpath,
		                   boost::asio::placeholders::bytes_transferred, 
		                   boost::asio::placeholders::error); 

	uint32_t buffSize = srcStream.getReadChunkSize();
        std::vector<char> buf(buffSize);
        srcStream.readAsync(buf, bufSize, handler);

	return outcome::success();
    }
    /*
     * Put the local file to another device
     */
    std::outcome<void> IOManager::putFile(const std::string& fpath, const std::string& dstPath) {
        std::shared_ptr<IODevice> dstDevice = nullptr;	

	// destination device
        schemaPos = dstPath.find("://");
        if (schemaPos == std::string::npos) {
            throw std::range_error("Dest Path is not correct");
        }
        auto dstSchema = dstPath.substr(0, schemaPos);
        it = io_devices_.find(dstSchema);
        if (it == io_devices_.end()) {
            throw std::range_error("No Device registered for the dst schema");
        }
        dstDevice = it->second();

	auto flags = StreamMode::WRITE_ONLY | StreamMode::APPEND;
	auto dstStream = dstDevice->open(dstPath, flags);
        if (dstStream == nullptr) {
            throw std::range_error("Dest device open failed");
        }
	// check if async is supported
	if(!dstStream->isAsyncSupported()) {
            throw std::range_error("Device does not support async write");
	}

        auto handler = boost::bind(&async_channel_.onPutComplete,
                                   this,
                                   fpath,
                                   boost::asio::placeholders::bytes_transferred,
                                   boost::asio::placeholders::error);

	// read file
	std::ifstream srcFile(fpath, std::ios::binary);
	if(!srcFile) {
            // error
	}
	srFile.seekg(0, std::ios::end);
	std::streampos fileSize = strFile.tellg();
	srcFile.seekg(0, std::ios::beg);
	std::vector<char> buffer(fileSize);
	if (infile.read(buffer.data(), fileSize)) {
	    
	}

        srcStream.writeAsync(buffer(data), fileSize, handler);

	return outcome::success();
    }

}
