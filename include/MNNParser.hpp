#ifndef MNNPARSER_HPP
#define MNNPARSER_HPP

#include <memory>
#include "FileParser.hpp"

class MNNParser : public FileParser {
    SINGLETON_PTR(MNNParser);
public:
    virtual shared_ptr<void> ParseData(shared_ptr<void> data) override;
};

#endif  // MNNPARSER_HPP
