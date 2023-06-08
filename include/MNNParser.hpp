#ifndef MNNPARSER_HPP
#define MNNPARSER_HPP

#include <memory>
#include "FileParser.hpp"

class MNNParser : public FileParser {
private:
    static MNNParser *instance;
    MNNParser() {}
public:
    static MNNParser *GetInstance();
    virtual void *ParseData(void *data) override;
};

#endif  // MNNPARSER_HPP
