// FileSaver.hpp

#ifndef FILESAVER_HPP
#define FILESAVER_HPP

#include <string>
#include <memory>

using namespace std;

class FileSaver {
public:
    virtual ~FileSaver() {}
    virtual void SaveFile(std::string filename, shared_ptr<void> data) = 0;
};

#endif

