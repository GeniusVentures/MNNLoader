#include <iostream>
#include <string>
#include "MNNLoader.hpp"
#include "FileManager.hpp"
#include "URLStringUtil.h"

/**
 * This program is example to loading MNN model file
 */
#define IPFS_FILE_PATH_NAME "ipfs://example.mnn"
#define FILE_PATH_NAME "file://../test/1.mnn"
#define FILE_SAVE_NAME "file://test.mnn"

int main(int argc, char **argv)
{
    std::string file_name = "";
    if (argc != 2)
    {
        std::cout << argv[0] << " [MNN extension file]" << std::endl;
        std::cout << "E.g:" << std::endl;
        std::cout << "\t " << argv[0] << " file://../test/1.mnn" << std::endl;
        return 1;
    }
    else
    {
        file_name = std::string(argv[1]);
    }

    // this should all be moved to GTest
    // Break out the URL prefix, file path, and extension.
    std::string url_prefix;
    std::string file_path;
    std::string extension;

    getURLComponents(
            "https://www.example.com/what is this and who cares about the extension anyway?dum='.jpg'",
            url_prefix, file_path, extension);
    // Print the results.
    cout << "urlPrefix: " << url_prefix << endl;
    cout << "filePath: " << file_path << endl;
    cout << "extension: " << extension << endl;

    getURLComponents("file://./test.mnn", url_prefix, file_path, extension);
    // Print the results.
    cout << "urlPrefix: " << url_prefix << endl;
    cout << "filePath: " << file_path << endl;
    cout << "extension: " << extension << endl;

    getURLComponents("ifps://really/long/path/to/a/file/testme.jpg",
            url_prefix, file_path, extension);
    // Print the results.
    cout << "urlPrefix: " << url_prefix << endl;
    cout << "filePath: " << file_path << endl;
    cout << "extension: " << extension << endl;

    // test plugins
    if (file_name.empty())
    {
        file_name = FILE_PATH_NAME;
    }
    std::cout << "LoadFile: " << file_name << std::endl;
    auto data = FileManager::GetInstance().LoadFile(file_name);
    std::cout << "LoadFile with Parse: " << file_name << std::endl;
    data = FileManager::GetInstance().LoadFile(file_name, true);
    std::cout << "ParseFile: " << std::endl;
    FileManager::GetInstance().ParseData("mnn", data);
    std::cout << "SaveFile: Save to " << FILE_SAVE_NAME;
    FileManager::GetInstance().SaveFile(FILE_SAVE_NAME, data);
    std::cout << std::endl;

    return 0;
}

