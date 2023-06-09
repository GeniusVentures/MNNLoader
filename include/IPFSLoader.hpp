// IPFSLoader.hpp

#ifndef IPFSLOADER_HPP
#define IPFSLOADER_HPP

#include "FileLoader.hpp"
#include "Singleton.hpp"

/// @brief class to handle "ipfs://" prefix in a filename to load from ipfs
class IPFSLoader : public FileLoader {
    SINGLETON_PTR(IPFSLoader);
public:
    /// @brief Load a file from somewhere based on prefix
    /// @param filename URL prefix encoded filename, i.e. https://filename.html, ipfs://filename.mnn
    /// @return a void pointer to the data associated with loading the file into memory
    virtual std::shared_ptr<void> LoadFile(std::string filename) override;

};

#endif
