#ifndef SGNS_FILEDEVICE_HPP
#define SGNS_FILEDEVICE_HPP

#include <string>
#include "IODevice.hpp"
#include "IOStream.hpp"

using FileSource = boost::iostreams::file_source;
using FileSink   = boost::iostreams::file_sink;

namespace sgns::io {
    
    // Device for Streaming Local files
    // file:// schema
    class FileDevice: public IODevice {
    public:
	FileDevice();
	virtual ~FileDevice() {}

        // open the device
	std::shared_ptr<IOStream> open(const std::string& path, 
		       const IOStream::StreamDirection& dir, 
		       const IOStream::StreamFlags& flags) override;
	void close() override; 

    private:
	std::shared_ptr<IOStream> getInStream_(const FileSource& source);
	std::shared_ptr<IOStream> getOutStream_(const FileSink& sink);

	std::shared_ptr<FileSource> src_;
	std::shared_ptr<FileSink>   dst_;

    };

}

#endif // SGNS_FILEDEVICE_HPP


