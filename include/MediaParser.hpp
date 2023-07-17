#ifndef SGNS_MEDIAPARSER_HPP
#define SGNS_MEDIAPARSER_HPP

#include <string>
#include <memory>

class MediaParser {
public:
    enum MediaType = {
         MEDIA_JSON = "json",
	 MEDIA_JPG  = "jpg",
	 MEDIA_MP3  = "mp3"
    };

    virtual ~MediaParser() {}
    virtual std::shared_ptr<void> ParseData(std::shared_ptr<void> data) = 0;
};

#endif
