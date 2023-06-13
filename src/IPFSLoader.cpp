// IPFSLoader.cpp

#include "FileManager.hpp"
#include "IPFSLoader.hpp"

SINGLETON_PTR_INIT(IPFSLoader);

IPFSLoader::IPFSLoader() {
    FileManager::GetInstance().RegisterLoader("ipfs", this);
}

std::shared_ptr<void> IPFSLoader::LoadFile(std::string filename) {
    const char *dummyValue = "Inside the IPFSLoader::LoadFile Function";
    // for this test, we don't need to delete the shared_ptr as the data is static, so pass null lambda delete function
    return {(void *)dummyValue, [](void *) {} };
    /* TODO: scorpioluck20 - Need to implement this. How we load file base on format file?*/
}

