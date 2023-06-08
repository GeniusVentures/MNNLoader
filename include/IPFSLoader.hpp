// IPFSLoader.hpp

#ifndef IPFSLOADER_HPP
#define IPFSLOADER_HPP

#include "FileLoader.hpp"

class IPFSLoader : public FileLoader {
private:
    static IPFSLoader *instance;
    IPFSLoader() {}
public:
    virtual void *LoadFile(string filename) override;

    static IPFSLoader *GetInstance();
};

#endif
