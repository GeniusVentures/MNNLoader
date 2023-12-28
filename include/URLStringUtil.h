//
// Created by Super Genius on 6/12/23.
//

#ifndef URLSTRINGUTIL_H
#define URLSTRINGUTIL_H

#include <string>

/// @brief extract the URL prefix from string
/// @param url string with prefix, i.e. "https://"
/// @return prefix, i.e. "https"
extern void getURLComponents(std::string url, std::string &prefix, std::string& base, std::string& extension);
extern void parseHTTPUrl(std::string url, std::string& host, std::string& path, std::string& port);
extern void parseSFTPUrl(std::string url, std::string& host, std::string& path, std::string& user, std::string& pass, std::string& key_file);

#endif // URLSTRINGUTIL_H
