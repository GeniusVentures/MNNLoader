
#include "FileManager.hpp"
#include "URLStringUtil.h"
#include <cassert>

namespace sgns::io {

    /*
     * Register the device
     * @schema - path schema
     * @device - Device that handles the schema media 
     */
    void IOManager::registerDevice(const string& schema, 
		    std::shared_ptr<IODevice> device) {
	io_devices_[schema] = device;    
    }

    // register parser
    void IOManager::registerParser(const MediaType& type, MediaParser *parser) {
        media_parsers_[type] = parser;
    }

    /*
     * Get the file from the device and write to local file  
     */
    std::outcome<void> IOManager::getFile(const std::string& srcPath, const std::string& fpath) {
        // append 'file://' for the local file
	std::string dstPath = "file://" + fpath;
	return moveMedia_(srcPath, dstPath);

    }
    /*
     * Put the local file to another device
     */
    std::outcome<void> IOManager::putFile(const std::string& fpath, const std::string& dstPath) {
	std::string srcPath = "file://" + fpath;
        return moveMedia_(srcPath, dstPath); 
    }
    /*
     * Move the file between devices
     */
    std::outcome<void> IOManager::moveFile(const std::string& srcPath, const string& dstPath) {
        return moveMedia_(srcPath, dstPath); 
    }	    

    /*
     * Move the Media between devices
     * @srcPath file path
     * @dstPath file path
     * @parse flag
     */
    std::outcome<void> IOManager::moveMedia_(const std::string& srcPath, const string& dstPath) {
	bool found = false;
	std::shared_ptr<IODevice> srcDevice = nullptr;
	std::shared_ptr<IODevice> dstDevice = nullptr;

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

	// Open the input device
	// flags if any
	auto srcStream = srcDevice->open(srcPath, StreamMode::READ_ONLY);
	if (srcStream == nullptr) {
            throw std::range_error("Src device open failed");
	}
	// Open the output device
	auto flags = StreamMode::WRITE_ONLY | StreamMode::APPEND;
	if(srcStream.isBinary()) {
	    flags |= StreamMode::BINARY; 
	}
	auto dstStream = dstDevice->open(dstPath, flags); 
	if (dstStream == nullptr) {
            throw std::range_error("Dest device open failed");
	}

	uint32_t buffSize = srcStream.getReadChunkSize();
	std::vector<char> buf(buffSize);
	while(srcStream.read(buf, bufSize)) {
	    // write to the destination
	    dstStream.write(buf, bufSize);
	}

	srcDevice.close();
	dstDevice.close();

	return outcome::success();
    }


    shared_ptr<void> IOManagerManager::ParseData(const std::string& suffix, shared_ptr<void> data) {
        auto parserIter = parsers.find(suffix);
        if (parserIter == parsers.end()) {
            throw std::range_error("No parser registered for suffix " + suffix);
        }
    
        auto parser = dynamic_cast<FileParser *>(parserIter->second);
        data = parser->ParseData(data);
        return data;
    }
