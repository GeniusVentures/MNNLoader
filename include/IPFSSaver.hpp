// IPFSSAVER.hpp

#ifndef IPFSSAVER_HPP
#define IPFSSAVER_HPP

#include "FileSaver.hpp"
#include "Singleton.hpp"

/// @brief class to handle "ipfs://" prefix in a filename to save to ipfs
class IPFSSaver : public FileSaver {
    SINGLETON_PTR(IPFSSaver);
public:
    /// @brief save a file to ipfs, throws on error
    /// @param filename filename to save the file as
    virtual void SaveFile(std::string filename, std::shared_ptr<void> data) override;

};

#endif
