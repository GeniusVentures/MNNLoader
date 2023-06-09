// FileLoaders.hpp

#ifndef FILELOADER_HPP
#define FILELOADER_HPP

#include <string>

class FileLoader {
public:
    /// @brief virtual destructor to prevent memory leaks from derived classes
    virtual ~FileLoader() {}
    /// @brief Load a file into memory
    /// @param filename URL prefix based filename to load from, i.e. 'https://filename.html', 'ipfs://testme.mnn', etc.
    /// @return a shared void pointer to the in memory data that was loaded, auto deletes at termination
    virtual std::shared_ptr<void> LoadFile(std::string filename) = 0;
};

#endif

