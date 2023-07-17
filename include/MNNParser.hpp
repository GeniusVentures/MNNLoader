#ifndef MNNPARSER_HPP
#define MNNPARSER_HPP

#include <memory>
#include "MediaParser.hpp"

class MNNParser : public MediaParser {
    SINGLETON_PTR(MNNParser);
public:
    virtual shared_ptr<void> ParseData(shared_ptr<void> data) override;
};

#endif  // MNNPARSER_HPP
