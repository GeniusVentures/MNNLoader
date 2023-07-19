#ifndef SGNS_MEDIAFILTER_HPP
#define SGNS_MEDIAFILTER_HPP

#include <string>
#include <memory>

/*
 * Media Filter
 * Stream filters that can be applied on Input & Output devicestreams 
 * After reading from device or before writing to the device.
 */
namespace sgns::io {

    class MediaFilter {
    public:
        using StreamSize = std::streamsize;

	enum FilterType {
          FILTER_GZIP = 1,
	};

        virtual ~MediaFilter() {}
        virtual StreamSize read(char* s, StreamSize n) = 0;
        virtual StreamSize write(const char* s, StreamSize n) = 0;

    private:

    };

}

#endif // SGNS_MEDIAFILTER_HPP
~                               
