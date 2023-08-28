#ifndef SGNS_MNNDEVICE_HPP
#define SGNS_MNNDEVICE_HPP

#include <string>
#include "IODevice.hpp"
#include "IOStream.hpp"
#include <MNN/MNNDefine.h>
#include <MNN/Interpreter.hpp>
#include <MNN/Tensor.hpp>


namespace sgns::io {

    // Device for Streaming Local files
    // file:// schema
    class MNNDevice: public IODevice {
    public:
        MNNDevice();
        virtual ~MNNDevice() {}

        // open the device
        std::shared_ptr<IOStream> open(const std::string& path,
                       const IOStream::StreamDirection& dir,
                       const IOStream::StreamFlags& flags) override;
        void close() override;

    private:
	std::shared_ptr<MNN::Interpreter> m_mnn_context; /* The holder of the model data */

    };

}

#endif // SGNS_MNNDEVICE_HPP
