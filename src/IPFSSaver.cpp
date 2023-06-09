// IPFSLoader.cpp

#include <iostream>
#include "FileManager.hpp"
#include "IPFSSaver.hpp"

SINGLETON_PTR_INIT(IPFSSaver);

IPFSSaver::IPFSSaver() {
    FileManager::GetInstance().RegisterSaver("ipfs:", this);
}

void IPFSSaver::SaveFile(std::string filename, std::shared_ptr<void> data) {
    std::cout << (char *)data.get() << " -> Inside the IPFSSaver::SaveFile Function";
}

