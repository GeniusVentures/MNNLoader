// FileLoaders.hpp

#ifndef FILELOADER_HPP
#define FILELOADER_HPP

#include <string>
#include "boost/asio.hpp"

class FileLoader {
public:
    /// @brief virtual destructor to prevent memory leaks from derived classes
    virtual ~FileLoader() {}
    /// @brief Load a file into memory
    /// @param filename URL prefix based filename to load from, i.e. 'https://filename.html', 'ipfs://testme.mnn', etc.
    /// @return a shared void pointer to the in memory data that was loaded, auto deletes at termination
    virtual std::shared_ptr<void> LoadFile(std::string filename) = 0;
    virtual std::shared_ptr<void> LoadASync(std::string filename, bool parse, std::shared_ptr<boost::asio::io_context> ioc) = 0;
};

#endif

