#ifndef SGNS_MEDIAPARSER_HPP
#define SGNS_MEDIAPARSER_HPP

#include <string>
#include <memory>

namespace sgns::io {

    class MediaParser {
    public:
        enum class MediaType {
          MEDIA_JSON = 1,
    	  MEDIA_JPG  = 2,
    	  MEDIA_MP3  = 3
        };
    
        virtual ~MediaParser() {}
        virtual std::shared_ptr<void> ParseData(std::shared_ptr<void> data) = 0;
    };

}

#endif
