#include <iostream>
#include <fstream>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include "FileDevice.hpp"
#include "FileStream.hpp"
#include "IOException.hpp"

using namespace std;

namespace sgns::io {
    
    FileDevice::FileDevice() {
        // init
    }

    std::shared_ptr<IOStream> FileDevice::open(const string& path,
		                               const IOStream::StreamDirection& dir,
		                               const IOStream::StreamFlags& flags) {
	// open the device 
	if (dir == IOStream::StreamDirection::READ_ONLY) {
	    src_ = make_shared<FileSource>(path);
	    if(!src_->is_open()) {
                throw IOException("Failed to open Source file"); 
	    }
	    return getInStream_(*src_);
	} else if (dir == IOStream::StreamDirection::WRITE_ONLY) {
	    dst_ = make_shared<FileSink>(path);
	    if(!dst_->is_open()) {
                throw IOException("Failed to open destination file"); 
	    }
	    return getOutStream_(*dst_);
	}
    }


    std::shared_ptr<IOStream> FileDevice::getInStream_(const FileSource& source) {
        std::shared_ptr<IOStream> in_stream = std::make_shared<FileStream>(source);
	return in_stream;
    }

    std::shared_ptr<IOStream> FileDevice::getOutStream_(const FileSink& sink) {
        // 
        std::shared_ptr<IOStream> out_stream = std::make_shared<FileStream>(sink);
        return out_stream;
    }

    void FileDevice::close() {

    }

}
