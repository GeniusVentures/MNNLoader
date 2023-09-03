#ifndef SGNS_ASYNC_MNNDEVICE_HPP
#define SGNS_ASYNC_MNNDEVICE_HPP

#include "AsyncIODevice.hpp"

#include <boost/asio.hpp>

using namespace boost::asio;

namespace sgns::io{
    class AsyncMNNDevice: public AsyncIODevice {
    public:
        using AsioContext = boost::asio::io_context;


        virtual ~AsyncMNNDevice();

        // open the device
        std::shared_ptr<IOStream> open(AsioContext& io_context,
                                       const std::string& path,
                                       const IOStream::StreamDirection& dir,
                                       const IOStream::StreamFlags& flags) override;
        void close() override;

    private:
        std::shared_ptr<posix::stream_descriptor> m_stream_file;

    };
}



#endif
