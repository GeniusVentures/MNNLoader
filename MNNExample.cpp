#include <iostream>
#include <string>
#include "MNNLoader.hpp"

/**
 * This program is example to loading MNN model file
 */
#define FILE_PATH_NAME "./test/example.mnn"

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
  sgns::MNNLoader *loader = new sgns::MNNLoader(file_name);
  if (loader == nullptr)
  {
    std::cout << "Can not loading the " << FILE_PATH_NAME << std::endl;
    return 1;
  }
  std::string info = loader->get_info();
  std::cout << info << std::endl;
  return 0;
}

