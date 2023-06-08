// FileParsers.hpp

#ifndef FILEPARSERS_HPP
#define FILEPARSERS_HPP

#include <string>

using namespace std;

class FileParser {
public:
    virtual ~FileParser() {}

    virtual void *ParseData(void *data) = 0;
};

#endif
