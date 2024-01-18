/**
 * Header file for the MNNLoader
 */

#ifndef INCLUDE_MNNLOADER_HPP_
#define INCLUDE_MNNLOADER_HPP_
#include <memory>
#include <string>
#include "FileLoader.hpp"
#include "MNNCommon.hpp"
#include "Singleton.hpp"
//#include "boost/asio/posix/stream_descriptor.hpp"


namespace sgns
{

    /**
     * This class is for loading files from Local Disk
     */
    class MNNLoader: public FileLoader
    {
        SINGLETON_PTR (MNNLoader);
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
            /**ok
             * Load Data on the MNN file
             * @param filename - MNN file part
             * @return Interpreter of MNN file
             *
             */
            std::shared_ptr<void> LoadFile(std::string filename) override;
            /**
             * Asynchronously load a file
             * @param filename - Filename to load
             * @param parse - Whether to parse file upon completion (for MNN)
             * @param save - Whether to save the file to local disk upon completion
             * @param ioc - ASIO context for async loading
             * @param callback - Filemanager callback on completion
             * @param status - Status function that will be updated with status codes as operation progresses
             * @return String indicating init
             */
            std::shared_ptr<void> LoadASync(std::string filename, bool parse, bool save, std::shared_ptr<boost::asio::io_context> ioc, CompletionCallback callback, StatusCallback status) override;
        protected:

    };
//#if defined(BOOST_ASIO_WINDOWS)
//    using FileStream = boost::asio::stream_file;
//#else
//    using FileStream = boost::asio::posix::stream_descriptor;
//#endif
//
//    class PlatformFileStream {
//    private:
//        std::unique_ptr<FileStream> fileStream_;
//
//    public:
//        PlatformFileStream() : fileStream_(std::make_unique<FileStream>()) {}
//
//        void open(const std::string& filename) {
//            fileStream_->open(filename, boost::asio::stream_file::flags::read_only);
//        }
//
//        std::size_t size() const {
//            return fileStream_->size();
//        }
//
//        auto& getFileStream() {
//            return *fileStream_;
//        }
//    };
} // End namespace sgns

#endif /* INCLUDE_MNNLOADER_HPP_ */
