//#define BOOST_ASIO_HAS_IO_URING 1 //Linux
//#define BOOST_ASIO_DISABLE_EPOLL 0 //Maybe linux?
#include <iostream>
#include <string>
#include <vector>
#include "Singleton.hpp"
#include "FileManager.hpp"
#include "MNNLoader.hpp"
#include "IPFSLoader.hpp"
#include "HTTPLoader.hpp"
#include "SFTPLoader.hpp"
#include "WSLoader.hpp"
#include "URLStringUtil.h"



/**
 * This program is example to loading MNN model file
 */
#define IPFS_FILE_PATH_NAME "ipfs://example.mnn"
#define FILE_PATH_NAME "file://../test/1.mnn"
#define FILE_SAVE_NAME "file://test.mnn"
void loadhandler(boost::system::error_code ec, std::size_t n, std::vector<char>& buffer) {
    if (!ec) {
        std::cout << "Received data: ";
        std::cout << std::endl;
        std::cout << "Handler Out Size:" << n << std::endl;
    }
    else {
        std::cerr << "Error in async_read: " << ec.message() << std::endl;
    }
}


int main(int argc, char **argv)
{
    std::string file_name = "";
    std::vector<string> file_names(0);
    std::cout << "argcount" << argc << std::endl;
    if (argc < 2)
    {
        std::cout << argv[0] << " [MNN extension file]" << std::endl;
        std::cout << "E.g:" << std::endl;
        std::cout << "\t " << argv[0] << " file://../test/1.mnn" << std::endl;
        return 1;
    }
    else
    {
        file_name = std::string(argv[1]);
        for (int i = 1; i < argc; i++)
        {
            std::cout << "file: " << argv[i] << std::endl;
            file_names.push_back(argv[i]);
        }
    }
   // return 1;
    // this should all be moved to GTest
    // Break out the URL prefix, file path, and extension.
    std::string url_prefix;
    std::string file_path;
    std::string extension;
    std::string http_host;
    std::string http_path;

    getURLComponents(
            "https://www.example.com/test.jpg",
            url_prefix, file_path, extension);
     //Print the results.
    //cout << "urlPrefix: " << url_prefix << endl;
    //cout << "filePath: " << file_path << endl;
    //cout << "extension: " << extension << endl;
    //parseHTTPUrl(file_path, http_host, http_path);
    //std::cout << "http host:" << http_host << std::endl;
    //std::cout << "http path:" << http_path << std::endl;

   // getURLComponents("file://./test.mnn", url_prefix, file_path, extension);
    // Print the results.
   // cout << "urlPrefix: " << url_prefix << endl;
    //cout << "filePath: " << file_path << endl;
    //cout << "extension: " << extension << endl;

   // getURLComponents("ipfs://really/long/path/to/a/file/testme.jpg",
     //       url_prefix, file_path, extension);
    // Print the results.
    //cout << "urlPrefix: " << url_prefix << endl;
    //cout << "filePath: " << file_path << endl;
    //cout << "extension: " << extension << endl;
    std::cout << "hmm" << std::endl;


    // test plugins
    if (file_name.empty())
    {
        file_name = FILE_PATH_NAME;
    }
    auto ioc = std::make_shared<boost::asio::io_context>();
    //boost::asio::io_context::strand strand(ioc);

    for (int i = 0; i < file_names.size(); i++)
    {
        std::cout << "LoadASync: " << file_names[i] << std::endl;
        auto data = FileManager::GetInstance().LoadASync(file_names[i],true,ioc);
        //std::cout << "LoadFile: " << file_names[i] << std::endl;
        //auto data = FileManager::GetInstance().LoadFile(file_names[i], false);

        //std::cout << "LoadFile with Parse: " << file_names[i] << std::endl;
        //data = FileManager::GetInstance().LoadFile(file_names[i], true);
        //std::cout << "ParseFile: " << std::endl;
        //FileManager::GetInstance().ParseData("mnn", data);
        //std::cout << "SaveFile: Save to " << FILE_SAVE_NAME;
        //FileManager::GetInstance().SaveFile(FILE_SAVE_NAME, data);
        //std::cout << std::endl;
        //boost::asio::post(ioc, postload);
    }
    
    ioc->run();

    return 0;
}

