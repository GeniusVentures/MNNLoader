cmake_minimum_required(VERSION 3.5.1)

project(FileLoader C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)

# Print debug current flag
message(STATUS "C flags: ${CMAKE_C_FLAGS}")
message(STATUS "CXX flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "C Debug flags: ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "CXX Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "C Release flags: ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "CXX Release flags: ${CMAKE_CXX_FLAGS_RELEASE}")


# ----- This is some option flag we can be defined
option(TESTING "Build tests" OFF)

if (TESTING)
    find_package(GTest CONFIG REQUIRED)
endif ()


# BOOST VERSION TO USE
set(BOOST_MAJOR_VERSION "1" CACHE STRING "Boost Major Version")
set(BOOST_MINOR_VERSION "80" CACHE STRING "Boost Minor Version")
set(BOOST_PATCH_VERSION "0" CACHE STRING "Boost Patch Version")
# convenience settings
set(BOOST_VERSION "${BOOST_MAJOR_VERSION}.${BOOST_MINOR_VERSION}.${BOOST_PATCH_VERSION}")
set(BOOST_VERSION_3U "${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}_${BOOST_PATCH_VERSION}")
set(BOOST_VERSION_2U "${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}")
# --------------------------------------------------------
# Set config of Boost project
set(_BOOST_ROOT "${THIRDPARTY_BUILD_DIR}/boost/build/${CMAKE_SYSTEM_NAME}${ABI_SUBFOLDER_NAME}")
set(Boost_LIB_DIR "${_BOOST_ROOT}/lib")
set(Boost_INCLUDE_DIR "${_BOOST_ROOT}/include/boost-${BOOST_VERSION_2U}")
set(Boost_DIR "${Boost_LIB_DIR}/cmake/Boost-${BOOST_VERSION}")
set(boost_headers_DIR "${Boost_LIB_DIR}/cmake/boost_headers-${BOOST_VERSION}")
set(boost_random_DIR "${Boost_LIB_DIR}/cmake/boost_random-${BOOST_VERSION}")
set(boost_system_DIR "${Boost_LIB_DIR}/cmake/boost_system-${BOOST_VERSION}")
set(boost_filesystem_DIR "${Boost_LIB_DIR}/cmake/boost_filesystem-${BOOST_VERSION}")
set(boost_program_options_DIR "${Boost_LIB_DIR}/cmake/boost_program_options-${BOOST_VERSION}")
set(boost_date_time_DIR "${Boost_LIB_DIR}/cmake/boost_date_time-${BOOST_VERSION}")
set(boost_regex_DIR "${Boost_LIB_DIR}/cmake/boost_regex-${BOOST_VERSION}")
set(boost_atomic_DIR "${Boost_LIB_DIR}/cmake/boost_atomic-${BOOST_VERSION}")
set(boost_chrono_DIR "${Boost_LIB_DIR}/cmake/boost_chrono-${BOOST_VERSION}")
set(boost_log_DIR "${Boost_LIB_DIR}/cmake/boost_log-${BOOST_VERSION}")
set(boost_log_setup_DIR "${Boost_LIB_DIR}/cmake/boost_log_setup-${BOOST_VERSION}")
set(boost_thread_DIR "${Boost_LIB_DIR}/cmake/boost_thread-${BOOST_VERSION}")
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_NO_SYSTEM_PATHS ON)
option(Boost_USE_STATIC_RUNTIME "Use static runtimes" ON)

option (SGNS_STACKTRACE_BACKTRACE "Use BOOST_STACKTRACE_USE_BACKTRACE in stacktraces, for POSIX" OFF)
if (SGNS_STACKTRACE_BACKTRACE)
	add_definitions(-DSGNS_STACKTRACE_BACKTRACE=1)
	if (BACKTRACE_INCLUDE)
		add_definitions(-DBOOST_STACKTRACE_BACKTRACE_INCLUDE_FILE=${BACKTRACE_INCLUDE})
	endif()
endif ()
# header only libraries must not be added here
find_package(Boost REQUIRED COMPONENTS date_time filesystem random regex system thread log log_setup program_options)
include_directories(${Boost_INCLUDE_DIRS})
# ---------------------------
# Set default variable
# FileLoader defines the version
set(VERSION_STRING 0.0.1)
set(SOVERSION_STRING 0.0.1)

# root dir
set(FILELOADER_ROOT_DIR ${CMAKE_SOURCE_DIR})
# set default build dir

if (NOT DEFINED BUILD_FILELOADER_DIR)
    set(BUILD_FILELOADER_DIR ${FILELOADER_ROOT_DIR}/.build/FileLoader)
endif ()
message(STATUS "BUILD_FILELOADER_DIR: ${BUILD_FILELOADER_DIR}")
# Set source file dir
set(PROJECT.Path ${CMAKE_SOURCE_DIR}/src)
set(PROJECT.Source_DIR
    ${PROJECT.Path}
)

set(FileLoader_LIB "fileloader")
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_FILELOADER_DIR}/lib" )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_FILELOADER_DIR}/lib" )

SET(TESTAPP MNNExample )

# -----------------------------------
# Include external plugin need
include(ExternalProject)
include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/include
)

#set(MNN_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../MNN/include")
# ----------------------BUILD EXTERNAL PROJECT------------------
# Builds MNN
message(STATUS "MNN_INCLUDE_DIR: ${MNN_INCLUDE_DIR}")
if (NOT DEFINED MNN_INCLUDE_DIR)
	message(STATUS "Start building MNN library")
	ExternalProject_Add(MNN
	  PREFIX MNN
	  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../MNN"
	  CMAKE_CACHE_ARGS
	        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/MNN
	        -DMNN_BUILD_SHARED_LIBS:BOOL=OFF
	        -DMNN_WIN_RUNTIME_MT:BOOL=${_MNN_WIN_RUNTIME_MT}
	        ${_CMAKE_COMMON_BUILD_PARAMS}
	        ${_MNN_EXTRA_PARAM}
	)
	set(_FINDPACKAGE_MNN_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/MNN/lib/cmake/mnn")
	set(_FINDPACKAGE_MNN_LIBRARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/MNN/lib")
	set(_FINDPACKAGE_MNN_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/MNN/include")
	set(MNN_INCLUDE_DIR ${_FINDPACKAGE_MNN_INCLUDE_DIR})
	set(MNN_LIBRARY_DIR ${_FINDPACKAGE_MNN_LIBRARY_DIR})
	set(NEED_BUILD_MNN TRUE)	
endif()
include_directories(${MNN_INCLUDE_DIR})
message(STATUS "MNN_INCLUDE_DIR: ${MNN_INCLUDE_DIR}")
message(STATUS "MNN_LIBRARY_DIR: ${MNN_LIBRARY_DIR}")

# -------------- Include common function ----------------------------
include (cmake/common.cmake)


# ----------------Build library---------------------------------
# Set source file
foreach(dir_path ${PROJECT.Source_DIR})
    aux_source_directory(${dir_path} PROJECT_SRC_CODE)
endforeach()

file(GLOB PROJECT_LIB_LIST "${MNN_LIBRARY_DIR}/*")

# ------------------------------------------
# Build Binary
add_fileloader_library(${VERSION_STRING} ${SOVERSION_STRING})
if (DEFINED NEED_BUILD_MNN)
	# in case we need to build an MNN library. Just waiting
	message(STATUS "Add dependencies on MNN")
	add_dependencies(${FileLoader_LIB} MNN)
endif()
# -------------------------------------------
# Combine binary to execute the example

link_directories(${BUILD_FILELOADER_DIR}/lib)
add_fileloader_executable("${TESTAPP}")

# install example binary to bin
install(TARGETS ${TESTAPP} DESTINATION "${BUILD_FILELOADER_DIR}/bin")
