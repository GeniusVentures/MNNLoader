//
// Created by Super Genius on 6/12/23.
//

#include "URLStringUtil.h"
#include <stdexcept>

extern void getURLComponents(std::string url, std::string &prefix, std::string& base, std::string& extension)
{
    // Find the first occurrence of "://" in the URL.
    size_t index = url.find("://");

    // If "://" is not found, then the URL has no prefix.
    if (index == std::string::npos) {
        throw std::invalid_argument("url");
    }
    prefix = url.substr(0, index);
    base = url.substr(index+3, url.length());
    size_t start_index = base.rfind(".");
    if (start_index == std::string::npos) {
        extension = "";
    } else {
        extension = base.substr(start_index+1, base.length());
    }
}


