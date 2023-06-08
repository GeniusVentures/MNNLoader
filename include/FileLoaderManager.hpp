#ifndef FILELOADERMANAGER_HPP
#define FILELOADERMANAGER_HPP

#include <string>
#include <map>
#include <filesystem>

#include "FileLoader.hpp"
#include "FileParser.hpp"

using namespace std;
namespace fs = std::filesystem;

class FileLoaderManager {
public:
    static FileLoaderManager &GetInstance() {
        static FileLoaderManager instance;
        return instance;
    }

    void RegisterLoader(string prefix, FileLoader *handlerLoader) {
        loaders[prefix] = handlerLoader;
    }

    void RegisterParser(string suffix, FileParser *handlerParser) {
        parsers[suffix] = handlerParser;
    }

    void *LoadFile(string filePath, bool parse = false) {
        std::filesystem::path path(filePath);
        std::string prefix = path.parent_path().string();
        std::string filename = path.filename().string();
        std::string suffix = path.extension().string();

        if (loaders.find(prefix) == loaders.end()) {
            throw std::range_error("No loader registered for prefix " + prefix);
        }

        if (parse && (parsers.find(suffix) == parsers.end())) {
            throw std::range_error("No parser registered for suffix " + suffix);
        }

        void *data = (*loaders[prefix]).LoadFile(filename);
        if (parse) {
            data = (*parsers[suffix]).ParseData(data);
        }

        return data;
    }

private:
    map<string, FileLoader *> loaders;
    map<string, FileParser *> parsers;

    FileLoaderManager() {}
    FileLoaderManager(const FileLoaderManager &) = delete;
    FileLoaderManager &operator=(const FileLoaderManager &) = delete;
};

#endif
