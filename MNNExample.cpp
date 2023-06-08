#include <iostream>
#include <string>
//#include "MNNLoader.hpp"
#include "FileLoaderManager.hpp"

/**
 * This program is example to loading MNN model file
 */
#define IPFS_FILE_PATH_NAME "ipfs://example.mnn"
#define FILE_PATH_NAME "file://./test/example.mnn"

int main(int argc, char **argv)
{
  std::string file_name;
  if(argc != 2) {
    std::cout << argv[0] << " [MNN extension file]" << std::endl;
    return 1;
  }
  else {
    file_name = std::string(argv[1]);
  }

  std::cout << (char *)(FileLoaderManager::GetInstance().LoadFile(IPFS_FILE_PATH_NAME)) << std::endl;
  std::cout << (char *)(FileLoaderManager::GetInstance().LoadFile(IPFS_FILE_PATH_NAME, true)) << std::endl;

#if 0
  sgns::MNNLoader *loader = new sgns::MNNLoader(file_name);
  if (loader == nullptr)
  {
    std::cout << "Can not loading the " << FILE_PATH_NAME << std::endl;
    return 1;
  }
  std::string info = loader->get_info();
  std::cout << info << std::endl;
#endif
  return 0;
}

