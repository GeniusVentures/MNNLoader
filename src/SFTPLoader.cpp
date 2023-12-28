#include <sstream>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <string>
#include <memory>
#include <iostream>
#include "FileManager.hpp"
#include "SFTPLoader.hpp"
#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"
#include "URLStringUtil.h"
#include "libssh2.h"
#include "libssh2_sftp.h"

#ifdef _WIN32
    // Windows platform
    using StreamDescriptor = boost::asio::windows::stream_handle;
#else
    // Assume POSIX-like platform
    using StreamDescriptor = boost::asio::posix::stream_descriptor;
#endif

namespace sgns
{
    SINGLETON_PTR_INIT(SFTPLoader);
    SFTPLoader::SFTPLoader()
    {
        FileManager::GetInstance().RegisterLoader("sftp", this);
    }

    std::shared_ptr<void> SFTPLoader::LoadFile(std::string filename)
    {
        const char* dummyValue = "Inside the SFTPTLoader::LoadFile Function";
        // for this test, we don't need to delete the shared_ptr as the data is static, so pass null lambda delete function
        return { (void*)dummyValue, [](void*) {} };
        /* TODO: scorpioluck20 - Need to implement this. How we load file base on format file?*/
    }


    std::shared_ptr<void> SFTPLoader::LoadASync(std::string filename, bool parse, std::shared_ptr<boost::asio::io_context> ioc, CompletionCallback handle_read)
    {
            //Parse hostname and path
        std::string sftp_host;
        std::string sftp_path;
        std::string sftp_user;
        std::string sftp_pass;
        parseSFTPUrl(filename, sftp_host, sftp_path, sftp_user, sftp_pass);
        std::cout << "host " << sftp_host << std::endl;
        std::cout << "path " << sftp_path << std::endl;
        std::cout << "user " << sftp_user << std::endl;
        std::cout << "pass " << sftp_pass << std::endl;
        // Initialize libssh2
        libssh2_init(0);
        
        // Create an SSH session
        LIBSSH2_SESSION* session = libssh2_session_init();
        //Debugging Items for inspecting communications
        //libssh2_trace(session, LIBSSH2_TRACE_AUTH | LIBSSH2_TRACE_SOCKET | LIBSSH2_TRACE_TRANS);
        //libssh2_trace(session, LIBSSH2_TRACE_AUTH | LIBSSH2_TRACE_SOCKET);
        boost::asio::ip::tcp::resolver resolver(*ioc);
        auto const results = resolver.resolve(sftp_host, "22");

        auto tcpSocket = std::make_shared<boost::asio::ip::tcp::socket>(*ioc);

        boost::asio::async_connect(*tcpSocket, results, [session, tcpSocket, sftp_path, sftp_user, sftp_pass, ioc, handle_read, parse](const boost::system::error_code& connect_error, const auto& /*endpoint*/) {
            if (!connect_error)
            {
                // Connect to the SFTP server
                libssh2_socket_t sock;
                sock = tcpSocket->native_handle();
                libssh2_session_handshake(session, sock);
                libssh2_session_set_blocking(session, 1);

                // Authenticate with username and password
                int auth_result = libssh2_userauth_password(session, sftp_user.c_str(), sftp_pass.c_str());

                // Combine . and sftpPath to form the full path
                std::string fullPath = "." + sftp_path;

                // Open an SFTP channel
                LIBSSH2_SFTP* sftp = libssh2_sftp_init(session);
                LIBSSH2_SFTP_HANDLE* sftpHandle = libssh2_sftp_open(sftp, fullPath.c_str(), LIBSSH2_FXF_READ, 0);
                if (!sftpHandle) {
                    int sftp_error_code = libssh2_sftp_last_error(sftp);
                    std::cerr << "Error opening SFTP file handle: " << sftp_error_code << std::endl;
                }
                // Get the size of the file
                LIBSSH2_SFTP_ATTRIBUTES sftpAttrs;
                libssh2_sftp_stat(sftp, fullPath.c_str(), &sftpAttrs);
                std::size_t file_size = sftpAttrs.filesize;

                auto buffer = std::make_shared<std::vector<char>>(file_size);
                auto asyncSFTPRead = [ioc, handle_read, sftp, buffer, sftpHandle, parse]() {
                    // Synchronous SFTP read inside of async thread
                    int rc;
                    size_t totalBytesRead = 0;
                    while ((rc = libssh2_sftp_read(sftpHandle, buffer->data() + totalBytesRead, buffer->size() - totalBytesRead)) > 0) {
                        // Process 'rc' bytes from the buffer asynchronously
                        //std::cout << "Read " << rc << " bytes from SFTP" << std::endl;
                        totalBytesRead += rc;
                        if (totalBytesRead >= buffer->size()) {
                            // Post to the IO context if needed
                            ioc->post([ioc, handle_read, buffer, parse]() {
                                //std::ofstream file("sftpoutput.txt", std::ios::binary);
                                //file.write(buffer->data(), buffer->size());
                                //file.close();
                                // Process the data or trigger further asynchronous operations from the IO context
                                std::cout << "SFTP Finish" << std::endl;
                                handle_read(ioc, buffer, parse);
                                });
                            totalBytesRead = 0;
                        }
                    }

                    // Handle the end of file or error
                    if (rc < 0) {
                        // Log the end of file or error
                        std::cout << "SFTP read completed with result: " << rc << std::endl;
                        if (rc != LIBSSH2_FX_EOF) {
                            // Handle error
                            std::cerr << "SFTP read error: " << rc << std::endl;
                        }
                    }

                    // Cleanup resources
                    libssh2_sftp_close_handle(sftpHandle);
                    libssh2_sftp_shutdown(sftp);
                };
                std::future<void> sftpReadFuture = std::async(std::launch::async, asyncSFTPRead);
                sftpReadFuture.get();
            }
            else {
                std::cerr << "Error connecting to server: " << connect_error.message() << std::endl;
            }
            });
 

        std::shared_ptr<string> result = std::make_shared < string>("test");
        return result;
    }

} // End namespace sgns
