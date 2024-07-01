/**
 * Header file for FILEError
 */
#ifndef FILERROR_HPP
#define FILERROR_HPP

#include <boost/outcome.hpp>

namespace outcome = BOOST_OUTCOME_V2_NAMESPACE;
namespace sgns
{
    namespace AsyncError
    {
        enum class ErrorCode {
            ERR_CONN,
            ERR_HANDSHAKE,
            ERR_AUTH,
            ERR_SFTPHANDLER,
            ERR_SFTPOPEN,
            ERR_SFTPFILESIZE,
            ERR_READFAILED,
            ERR_HTTPGETFAIL,
            ERR_SSLHANDSHAKE,
            ERR_WEBSOCKHANDSHAKE,
            ERR_WEBSOCKGET,
            ERR_LOCALOPEN,
            ERR_STARTIPFS,
            ERR_IPFSBLOCKREAD,
            ERR_IPFSSUBBLOCK,
            ERR_IPFSLISTEN,
            ERR_NOADDRBITSWAP,
            ERR_NOPROVBITSWAP,
            ERR_BADDHTRES
        };
        using CustomResult = outcome::result<std::string, ErrorCode>;
    }
}

#endif