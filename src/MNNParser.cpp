// MNNParser.cpp

#include "FileLoaderManager.hpp"
#include "MNNParser.hpp"

MNNParser *MNNParser::instance = MNNParser::GetInstance();

MNNParser *MNNParser::GetInstance() {
    if (instance == nullptr) {
        instance = new MNNParser();
        FileLoaderManager::GetInstance().RegisterParser(".mnn", instance);
    }
    return instance;
}

void *MNNParser::ParseData(void *data) {
    static char *dummyValue = "Inside the MNNParser::ParseData Function";
    return dummyValue;
    // TODO: Implement the logic to parse the file data.
}