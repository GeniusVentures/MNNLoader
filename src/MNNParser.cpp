// MNNParser.cpp

#include <iostream>
#include "FileManager.hpp"
#include "MNNParser.hpp"

SINGLETON_PTR_INIT(MNNParser);

MNNParser::MNNParser() {
    FileManager::GetInstance().RegisterParser(".mnn", this);
}

std::shared_ptr<void> MNNParser::ParseData(std::shared_ptr<void> data) {
    const char *dummyValue = "Inside the MNNParser::ParseData Function";
    std::cout << (char *)data.get() << " -> ";
    // for this test, we don't need to delete the shared_ptr as the data is static, so pass null lambda delete function
    return {(void *)dummyValue, [](void *) {}};

    // TODO: Implement the logic to parse the file data.
}