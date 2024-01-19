#ifndef FILELOADERMANAGER_HPP
#define FILELOADERMANAGER_HPP

#if defined(__linux__) || defined(__APPLE__)
#define BOOST_ASIO_HAS_IO_URING 1
#include <liburing.h>
#endif
#include <iostream>
#include <string>
#include <map>
#include <filesystem>
#include <cassert>
#include <future>
#include <memory>
#include "Singleton.hpp"
#include "FileLoader.hpp"
#include "FileParser.hpp"
#include "FileSaver.hpp"
#include "boost/asio.hpp"
#include "boost/bind.hpp"


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

        int outstandingOperations_ = 0;
    public:
        /**
         * Completion callback template. We expect an io_context so the thread can be shut down if no outstanding async loads exist, and a buffer with the read information
         * @param ioc - asio io context so we can stop this if no outstanding async tasks remain
         * @param buffer - Contains data loaded
         * @param parse - Whether to parse file upon completion (for MNN)
         * @param save - Whether to save the file to local disk upon completion
         */
        using CompletionCallback = std::function<void(std::shared_ptr<boost::asio::io_context> ioc, std::shared_ptr<std::vector<char>> buffer, bool parse, bool save)>;
        /**
         * Status callback returns an error code as an async load proceeds
         * @param ioc - asio io context so we can stop this if no outstanding async tasks remain
         * @param buffer - Contains data loaded
         * @param parse - Whether to parse file upon completion (for MNN)
         * @param save - Whether to save the file to local disk upon completion
         */
        using StatusCallback = std::function<void(const int&)>;
        /// @brief Decrement operations counter so io_context thread can be shut down when all are complete.
        /// @param The io_context that we have been reading on
        void DecrementOutstandingOperations(std::shared_ptr<boost::asio::io_context> ioc);
        /// @brief Increment operations counter so io_context thread can be shut down when all are complete.
        void IncrementOutstandingOperations();
        shared_ptr<int> GetOutstandingOperationsPointer();
        /// @brief Register a synchronous loader class to handle a specific prefix
        /// @param prefix = "https", "file", etc from https://xxxxx
        /// @param handlerLoader Handler class object that can load the data
        void RegisterLoader(const std::string &prefix,
                FileLoader *handlerLoader);
        /// @brief Register a synchronous Parser class to handle a specific extension suffix
        /// @param suffix = ".mnn", ".jpg", etc from file://file.jpg
        /// @param handlerParser Handler class object that can parse the data
        void RegisterParser(const std::string &suffix,
                FileParser *handlerParser);
        /// @brief Register a synchronous saver class to handle a specific prefix
        /// @param prefix = "mnn", "file", etc from mnn://xxxxx
        /// @param handlerSaver Handler class object that can save the data
        void RegisterSaver(const std::string &prefix,
                FileSaver *handlerSaver);

        /**
         * Asynchronously load a file based on type
         * @param url - URL to load, will determine loader we use
         * @param parse - Whether to parse file upon completion (for MNN)
         * @param save - Whether to save the file to local disk upon completion
         * @param ioc - ASIO context for async loading
         * @param callback - Filemanager callback on completion
         * @param status - Status function that will be updated with status codes as operation progresses
         * @return String indicating init
         */
        shared_ptr<void> LoadASync(const std::string& url, bool parse, bool save, std::shared_ptr<boost::asio::io_context> ioc, CompletionCallback callback, StatusCallback status);

        /// @brief Load a file given a filePath and optional parse the data
        /// @param url the full path and filename to load
        /// @param parse bool on weather to parse the file or not
        /// @return shared pointer to void * of the data loaded
        shared_ptr<void> LoadFile(const std::string &url, bool parse = false);

        /// @brief Parse Data from a previously loaded file
        /// @param suffix the extension/suffix to know how to parse the data
        /// @param data
        /// @return shared pointer to void * of the data parsed
        shared_ptr<void> ParseData(const std::string &suffix,
                shared_ptr<void> data);

        /// @brief Save Data to a file via some system, throws exception on error
        /// @param url URL prefix filename and extension
        /// @param data shared pointer to void * of the data to save
        void SaveFile(const std::string &url, std::shared_ptr<void> data);
};

#endif
