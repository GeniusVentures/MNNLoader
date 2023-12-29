// IPFSLoader.cpp

#include <iostream>
#include "FileManager.hpp"
#include "IPFSSaver.hpp"

SINGLETON_PTR_INIT(IPFSSaver);

IPFSSaver::IPFSSaver() {
    FileManager::GetInstance().RegisterSaver("ipfs", this);
}

void IPFSSaver::SaveFile(std::string filename, std::shared_ptr<void> data) {
    std::cout << (char *)data.get() << " -> Inside the IPFSSaver::SaveFile Function";
}

void IPFSSaver::SaveASync(std::shared_ptr<boost::asio::io_context> ioc, std::function<void(std::shared_ptr<boost::asio::io_context> ioc)> handle_write, std::string filename, std::shared_ptr<std::vector<char>> data) {
    std::cout << (char*)data.get() << " -> Inside the IPFSSaver::SaveFile Function";
}
