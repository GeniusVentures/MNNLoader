#ifndef SGNS_FILESTREAM_HPP
#define SGNS_FILESTREAM_HPP

namespace sgns::io {

    class FileStream: public IOStream {

    public:
	IOStream& read(const char *buf, size_t bytes) override;
        size_t write(const char *buf, size_t bytes) override;

    private:
	std::ifstream inStream_;
	std::ofstream outStream_;

    }

}

#endif
