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
        struct Success {
            std::string message;
        };
        using CustomResult = outcome::result<Success, std::string>;
    }
}

#endif