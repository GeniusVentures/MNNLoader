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
	${Boost_LIBRARIES}
	OpenSSL::SSL 
	OpenSSL::Crypto
    graphsync
    ipfs_merkledag_service
    p2p::asio_scheduler
	p2p::p2p_logger
	p2p::p2p_default_network
	p2p::p2p_cares
	p2p::p2p_dnsaddr_resolver
	p2p::p2p_default_host
	p2p::p2p_gossip
	p2p::p2p_ping
	p2p::p2p_identify
	p2p::p2p_identify_proto
	p2p::p2p_basic_host
    p2p::p2p_multiaddress
	ipfs-lite-cpp::cbor
	ipfs-lite-cpp::graphsync
	ipfs-lite-cpp::ipld_node
	ipfs-lite-cpp::ipfs_blockservice
	ipfs-lite-cpp::ipfs_datastore_in_memory
	ipfs-bitswap-proto
	ipfs-bitswap-cpp 
	spdlog::spdlog
	crypt32
	)
#target_link_libraries( ${Boost_LIBRARIES})
#file(INSTALL ${FILELOADER_HEADER} DESTINATION ${CMAKE_BINARY_DIR}/FileLoader/include)
#include_directories(${CMAKE_BINARY_DIR}/FileLoader/include )
include_directories(../include )
#link_directories(${CMAKE_BINARY_DIR}/FileLoader/lib)