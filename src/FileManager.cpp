#include "FileManager.hpp"
#include "URLStringUtil.h"

void FileManager::RegisterLoader(const std::string &prefix,
        FileLoader *handlerLoader)
{
    loaders[prefix] = handlerLoader;
}

void FileManager::RegisterParser(const std::string &suffix,
        FileParser *handlerParser)
{
    parsers[suffix] = handlerParser;
}

void FileManager::RegisterSaver(const std::string &prefix,
        FileSaver *handlerSaver)
{
    savers[prefix] = handlerSaver;
}

shared_ptr<void> FileManager::LoadFile(const std::string &url, bool parse)
{
    std::string prefix;
    std::string filePath;
    std::string suffix;

    getURLComponents(url, prefix, filePath, suffix);
#if 0
    std::cout << "DEBUG: URL: " << url << " -prefix: " << prefix << " -filePath: " << filePath << " -suffix: " << suffix << std::endl;
#endif

    auto loaderIter = loaders.find(prefix);

    if (loaderIter == loaders.end())
    {
        throw std::range_error("No loader registered for prefix " + prefix);
    }

    auto loader = loaderIter->second;
    // double check pointer is to a FileLoader class
    assert(dynamic_cast<FileLoader*>(loader));

    shared_ptr<void> data = loader->LoadFile(filePath);
    if (parse)
    {
        data = ParseData(suffix, data);
    }

    return data;
}

shared_ptr<void> FileManager::ParseData(const std::string &suffix,
        shared_ptr<void> data)
{
    auto parserIter = parsers.find(suffix);
    if (parserIter == parsers.end())
    {
        throw std::range_error("No parser registered for suffix " + suffix);
    }

    auto parser = dynamic_cast<FileParser*>(parserIter->second);
    data = parser->ParseData(data);
    return data;
}

void FileManager::SaveFile(const std::string &url, std::shared_ptr<void> data)
{
    std::string prefix;
    std::string filePath;
    std::string suffix;

    getURLComponents(url, prefix, filePath, suffix);

    auto saverIter = savers.find(prefix);

    if (saverIter == savers.end())
    {
        throw std::range_error("No saver registered for prefix " + prefix);
    }

    auto saver = saverIter->second;
    // double check pointer is to a FileSaver class
    assert(dynamic_cast<FileSaver*>(saver));

    saver->SaveFile(filePath, data);
}

