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
        // Define a custom policy
        struct custom_policy {
            template <class Impl>
            static constexpr void wide_value_check(Impl&& self) {
                if (!static_cast<bool>(self))
                    throw std::runtime_error("No value present");
            }

            template <class Impl>
            static constexpr void wide_error_check(Impl&& self) {
                if (!self.has_error())
                    throw std::runtime_error("No error present");
            }

            template <class Impl>
            static constexpr void wide_exception_check(Impl&& self) {
                if (!self.has_exception())
                    throw std::runtime_error("No exception present");
            }

            template <class Impl>
            static constexpr void narrow_value_check(Impl&& self) noexcept {
                if (!static_cast<bool>(self))
                    self.error();
            }

            template <class Impl>
            static constexpr void narrow_error_check(Impl&& self) noexcept {
                if (!self.has_error())
                    self.value();
            }

            template <class Impl>
            static constexpr void narrow_exception_check(Impl&& self) noexcept {
                if (!self.has_exception())
                    self.value();
            }

            // The required on_* methods
            template <class Impl, class... Args>
            static constexpr void on_result_construction(Impl&& self, Args &&... args) noexcept {}

            template <class Impl, class... Args>
            static constexpr void on_result_copy_construction(Impl&& self, Args &&... args) noexcept {}

            template <class Impl, class... Args>
            static constexpr void on_result_move_construction(Impl&& self, Args &&... args) noexcept {}

            template <class Impl, class... Args>
            static constexpr void on_result_in_place(Impl&& self, Args &&... args) noexcept {}

            template <class Impl, class... Args>
            static constexpr void on_result_copy_assignment(Impl&& self, Args &&... args) noexcept {}

            template <class Impl, class... Args>
            static constexpr void on_result_move_assignment(Impl&& self, Args &&... args) noexcept {}
        };

        using CustomResult = outcome::result<std::string, ErrorCode, custom_policy>;
    }
}

#endif