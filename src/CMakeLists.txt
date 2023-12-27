#add_subdirectory(application)
file(GLOB FILELOADER_HEADER "../include/*.h*")
file(GLOB FILELOADER_SRCS "*.cpp")
FILE(GLOB MNN_LIBS "${MNN_LIBRARY_DIR}/*")
add_library(fileloader OBJECT
    ${FILELOADER_SRCS}
    )
add_dependencies(fileloader libssh2)
target_link_libraries(fileloader PRIVATE
	${LIBSSH2_LIBRARY_DIR}/libssh2.lib
	${MNN_LIBRARY_DIR}/MNN.lib 
	OpenSSL::SSL 
	OpenSSL::Crypto
    graphsync
    ipfs_merkledag_service
    p2p::asio_scheduler
	p2p::p2p_default_network
	p2p::p2p_cares
	p2p::p2p_dnsaddr_resolver
	p2p::p2p_default_host
	p2p::p2p_gossip
	ipfs-lite-cpp::cbor
	ipfs-lite-cpp::graphsync
	ipfs-lite-cpp::ipld_node
	spdlog::spdlog
    Boost::date_time
    Boost::regex
	crypt32
	)
#target_link_libraries( ${Boost_LIBRARIES})
#file(INSTALL ${FILELOADER_HEADER} DESTINATION ${CMAKE_BINARY_DIR}/FileLoader/include)
#include_directories(${CMAKE_BINARY_DIR}/FileLoader/include )
include_directories(../include )
#link_directories(${CMAKE_BINARY_DIR}/FileLoader/lib)