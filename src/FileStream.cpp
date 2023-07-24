#include "FileStream.hpp"
#include <boost/asio.hpp>

using namespace boost::asio;
using namespace boost::iostreams;

namespace sgns::io {

    FileStream::FileStream(const FileSource& src) 
        :in_stream_(src) {

    }

    FileStream::FileStream(const FileSink& sink) 
        :out_stream_(sink) {

    }

    FileStream::FileStream(std::shared_ptr< posix::stream_descriptor> stream_file)
	:stream_file_(stream_file) {
	
    }

    FileStream::FileStream(const FileSource& src, const FileSink& sink)
	:in_stream_(src),
	 out_stream_(sink) {

    }
	
    size_t FileStream::read(const char*, size_t bytes) {
        return 0;
    }
    size_t FileStream::write(const char*, size_t bytes) {
        return 0;
    }

    void FileStream::readAsync(char* buff, size_t bytes, OnGetCompleteCallback handler) {
	stream_file_->async_read_some(boost::asio::buffer(buff, bytes), handler);
    }

    size_t FileStream::writeAsync(char* buff, size_t bytes, OnWriteCompleteCallback handler) {
	stream_file_->async_write_some(boost::asio::buffer(buff, bytes), handler);
	return bytes;
    }

}
