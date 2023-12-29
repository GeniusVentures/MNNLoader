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

namespace sgns
{

    /**
     * This class is for parsing the information in an MNN model file.
     * If you want to use this class, we can inheritance from this class
     * and implement logic based on model info
     */
    class MNNLoader: public FileLoader
    {
        SINGLETON_PTR (MNNLoader);
        public:
            /// @brief Completion callback template. We expect an io_context so the thread can be shut down if no outstanding async loads exist, and a buffer with the read information
            /// @param io_context that we are using to async files. Data from the async load.
            using CompletionCallback = std::function<void(std::shared_ptr<boost::asio::io_context> ioc, std::shared_ptr<std::vector<char>> buffer, bool parse, bool save)>;
            /**ok
             * Load Data on the MNN file
             * @param filename - MNN file part
             * @return Interpreter of MNN file
             *
             */
            std::shared_ptr<void> LoadFile(std::string filename) override;
            std::shared_ptr<void> LoadASync(std::string filename, bool parse, bool save, std::shared_ptr<boost::asio::io_context> ioc, CompletionCallback callback) override;
        protected:

    };

} // End namespace sgns

#endif /* INCLUDE_MNNLOADER_HPP_ */
