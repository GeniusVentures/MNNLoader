#ifndef SGNS_IOSTREAM_HPP
#define SGNS_IOSTREAM_HPP

namespace sgns::io {
    
    class IOStream {

    public:
	enum class StreamDirection {
            READ_ONLY = 1, // input stream
	    WRITE_ONLY = 2, // output stream
	    READ_AND_WRITE = 3, // bi-directional
	};

	enum class StreamFlags {
            STREAM_APPEND = 1,
	    STREAM_BINARY = 2
	}

	IOStream(const IOStreamDirection& dir, const StreamFlags& flags);
        virtual ~IOStream = default;

        virtual void read(const char*, size_t bytes) = 0;
        virtual size_t write(const char*, size_t bytes) = 0;	

    private:
	StreamMode  mode_;
	StreamFlags flags_;

    };

}


#endif // SGNS_IOSTREAM_HPP
