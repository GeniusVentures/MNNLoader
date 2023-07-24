#ifndef SGNS_IOEXCEPTION_HPP
#define SGNS_IOEXCEPTION_HPP

#include <exception>
#include <string>

namespace sgns::io {
    
    // Device for Streaming Local files
    // file:// schema
    class IOException: public std::exception {
    public:
	IOException(const std::string& msg) : message(msg) {}
	virtual ~IOException() {}

	const char* what() const noexcept override {
            return message.c_str();
        }

    private:
	std::string message;

    };

}

#endif // SGNS_IOEXCEPTION_HPP


