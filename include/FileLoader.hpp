// FileLoaders.hpp

#ifndef FILELOADERS_HPP
#define FILELOADERS_HPP

#include <string>

using namespace std;

class FileLoader {
public:
    virtual ~FileLoader() {}

    virtual void *LoadFile(string filename) = 0;
};

#endif

