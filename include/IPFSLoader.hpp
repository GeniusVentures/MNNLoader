// IPFSLoader.hpp

#ifndef IPFSLOADER_HPP
#define IPFSLOADER_HPP

#include "FileLoader.hpp"
#include "Singleton.hpp"

/// @brief class to handle "ipfs://" prefix in a filename to load from ipfs
class IPFSLoader : public FileLoader {
    SINGLETON_PTR(IPFSLoader);
public:
    /// @brief Completion callback template. We expect an io_context so the thread can be shut down if no outstanding async loads exist, and a buffer with the read information
    /// @param io_context that we are using to async files. Data from the async load.
    using CompletionCallback = std::function<void(std::shared_ptr<boost::asio::io_context> ioc, std::shared_ptr<std::vector<char>> buffer, bool parse, bool save)>;
    /// @brief Load a file from somewhere based on prefix
    /// @param filename URL prefix encoded filename, i.e. https://filename.html, ipfs://filename.mnn
    /// @return a void pointer to the data associated with loading the file into memory
    virtual std::shared_ptr<void> LoadFile(std::string filename) override;
    virtual std::shared_ptr<void> LoadASync(std::string filename, bool parse, bool save, std::shared_ptr<boost::asio::io_context> ioc, CompletionCallback callback, std::function<void(const int&)> status) override;

};

#endif

