#ifndef FILELOADERMANAGER_HPP
#define FILELOADERMANAGER_HPP

#include <iostream>
#include <string>
#include <map>
#include <filesystem>
#include <cassert>
#include "Singleton.hpp"
#include "FileLoader.hpp"
#include "FileParser.hpp"
#include "FileSaver.hpp"

/// \brief FileManager class handles all the registration of the file loaders, parsers and savers and proxies the basic
///         functionality to the registered handlers
class FileManager
{
    SINGLETON_REF(FileManager)
        ;
    private:
        /// @brief a map from std::string to loader handlers
        map<std::string, FileLoader*> loaders;
        /// @brief a map from std::string to parser handlers
        map<std::string, FileParser*> parsers;
        /// @brief a map from std::string to saver handlers
        map<std::string, FileSaver*> savers;

    public:
        void RegisterLoader(const std::string &prefix,
                FileLoader *handlerLoader)
        {
            loaders[prefix] = handlerLoader;
        }

        void RegisterParser(const std::string &suffix,
                FileParser *handlerParser)
        {
            parsers[suffix] = handlerParser;
        }

        void RegisterSaver(const std::string &prefix, FileSaver *handlerSaver)
        {
            savers[prefix] = handlerSaver;
        }

        shared_ptr<void> LoadFile(const std::string &filePath, bool parse =
                false)
        {
            // i.e. 'https://filename.html', 'ipfs://testme.mnn', 'file://../test/1.mnn'
            const std::filesystem::path path(filePath);
            const std::string prefix = path.parent_path().string(); /* FIX: This is a wrong way to get the prefix of URL */
            const std::string filename = path.filename().string();
            const std::string filepathname = path.relative_path().string();
            const std::string suffix = path.extension().string();
#if 0
            std::cout << "DEBUG: filePath: " << filePath << " -prefix: " << prefix << " -filename: "
                    << filename << " -suffix: " << suffix
                    << " -filepathname: " << filepathname << std::endl;
#endif
            auto loaderIter = loaders.find(prefix);

            if (loaderIter == loaders.end())
            {
                throw std::range_error(
                        "No loader registered for prefix " + prefix);
            }

            auto loader = loaderIter->second;
            // double check pointer is to a FileLoader class
            assert(dynamic_cast<FileLoader*>(loader));

            shared_ptr<void> data = loader->LoadFile(filename);
            if (parse)
            {
                data = ParseData(suffix, data);
            }

            return data;
        }

        /// @brief Parse Data from a previously loaded file
        /// @param filePath URL prefix filename and extension, extension is used to locate parser handler
        /// @param data
        /// @return
        shared_ptr<void> ParseData(const std::string &suffix,
                shared_ptr<void> data)
        {
            auto parserIter = parsers.find(suffix);
            if (parserIter == parsers.end())
            {
                throw std::range_error(
                        "No parser registered for suffix " + suffix);
            }

            auto parser = dynamic_cast<FileParser*>(parserIter->second);
            data = parser->ParseData(data);
            return data;
        }

        /// @brief Save Data to a file via some system, throws exception on error
        /// @param filePath URL prefix filename and extension
        /// @param data
        void SaveFile(const std::string &filePath, std::shared_ptr<void> data)
        {
            std::filesystem::path path(filePath);
            std::string prefix = path.parent_path().string();
            std::string filename = path.filename().string();

            auto saverIter = savers.find(prefix);

            if (saverIter == savers.end())
            {
                throw std::range_error(
                        "No saver registered for prefix " + prefix);
            }

            auto saver = saverIter->second;
            // double check pointer is to a FileSaver class
            assert(dynamic_cast<FileSaver*>(saver));

            saver->SaveFile(filename, data);
        }

};

#endif
