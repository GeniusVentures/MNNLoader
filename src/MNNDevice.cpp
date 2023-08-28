#include "MNNDevice.hpp"
#include "MNNStream.hpp"
#include "IOException.hpp"


using namespace std;

namespace sgns::io {

    MNNDevice::MNNDevice() {

    }


    std::shared_ptr<IOStream> MNNDevice::open(const string& path,
                                              const IOStream::StreamDirection& dir,
                                              const IOStream::StreamFlags& flags) {
        return make_shared<MNNStream>(1);	

    }

}
