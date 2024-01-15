/**
 * Header file for the WSLoader
 */

#ifndef INCLUDE_HTTPLOADER_HPP_
#define INCLUDE_HTTPLOADER_HPP_

#include <memory>
#include <string>
#include "FileLoader.hpp"
//#include "MNNCommon.hpp"
#include "Singleton.hpp"

namespace sgns
{

    /**
     * This class is for loading files from HTTP
     */
    class HTTPLoader : public FileLoader
    {
        SINGLETON_PTR(HTTPLoader);
    public:
        /// @brief Completion callback template. We expect an io_context so the thread can be shut down if no outstanding async loads exist, and a buffer with the read information
        /// @param io_context that we are using to async files. Data from the async load.
        using CompletionCallback = std::function<void(std::shared_ptr<boost::asio::io_context> ioc, std::shared_ptr<std::vector<char>> buffer, bool parse, bool save)>;
        using StatusCallback = std::function<void(const int&)>;
        /**ok
         * Load Data on the MNN file
         * @param filename - MNN file part
         * @return Interpreter of MNN file
         *
         */
        std::shared_ptr<void> LoadFile(std::string filename) override;
        std::shared_ptr<void> LoadASync(std::string filename, bool parse, bool save, std::shared_ptr<boost::asio::io_context> ioc, CompletionCallback callback, StatusCallback status) override;
    protected:

    };

} // End namespace sgns

#endif /* INCLUDE_HTTPLOADER_HPP_ */
