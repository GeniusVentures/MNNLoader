// IPFSLoader.cpp

#include "FileLoaderManager.hpp"
#include "IPFSLoader.hpp"

IPFSLoader *IPFSLoader::instance = IPFSLoader::GetInstance();

IPFSLoader *IPFSLoader::GetInstance() {
    if (instance == nullptr) {
        instance = new IPFSLoader();
        FileLoaderManager::GetInstance().RegisterLoader("ipfs:", instance);
    }
    return instance;
}

void *IPFSLoader::LoadFile(string filename) {
    static char *dummyValue = "Inside the IPFSLoader::LoadFile Function";
    return dummyValue;
}

