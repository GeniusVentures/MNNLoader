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

extern void parseHTTPUrl(std::string url, std::string& host, std::string& path)
{
    // Find the first occurrence of "/" in the URL.
    size_t index = url.find("/");
    // If "/" is not found, then the URL has no prefix.
    if (index == std::string::npos) {
        throw std::invalid_argument("url");
    }
    host = url.substr(0, index);
    path = url.substr(index, url.length());
}

extern void parseSFTPUrl(std::string url, std::string& host, std::string& path, std::string& user, std::string& pass)
{
    // Find the first occurrence of "@@" in the URL.
    size_t index = url.find("@");
    // If "@" is not found, then we have no user/pass 
    if (index == std::string::npos) {
        user = "";
        pass = "";
    }
    else {
        user = url.substr(0, index);
        host = url.substr(index, url.length());
    }
    index = user.find(":");
    // If ":" is not found, then we have no password minimally 
    if (index == std::string::npos) {
        pass = "";
    }
    else {
        pass = user.substr(index+1, url.length());
        user = user.substr(0, index);
    }

    index = host.find("/");
    // If "/" is not found, then the URL is missing full path
    if (index == std::string::npos) {
        throw std::invalid_argument("url");
    }
    else {
        path = host.substr(index, url.length());
        host = host.substr(1, index-1);
    }
}

