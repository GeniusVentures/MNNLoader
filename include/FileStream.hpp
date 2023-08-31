#ifndef SGNS_FILESTREAM_HPP
#define SGNS_FILESTREAM_HPP

#include <string>
#include "IODevice.hpp"
#include "IOStream.hpp"
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>

using FileSource = boost::iostreams::file_source;
using FileSink   = boost::iostreams::file_sink;

namespace sgns::io {

    class FileStream: public IOStream {

    public:

	FileStream() = default;
	FileStream(const FileSource&);
	FileStream(const FileSink&);
	FileStream(std::shared_ptr<boost::asio::posix::stream_descriptor>);
	FileStream(const FileSource&, const FileSink&);

	size_t read(const char*, size_t bytes) override;
        size_t write(const char *buf, size_t bytes) override;

	// Async
	void readAsync(char*, size_t bytes, OnGetCompleteCallback handler);
	size_t writeAsync(char*, size_t bytes, OnWriteCompleteCallback handler);

    private:
	// sync io streams
	boost::iostreams::stream<FileSource> in_stream_;
        boost::iostreams::stream<FileSink>   out_stream_;
        // async io stream
	std::shared_ptr<boost::asio::posix::stream_descriptor> stream_file_;

    };

}

#endif
