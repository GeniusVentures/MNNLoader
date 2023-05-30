cmake_minimum_required(VERSION 3.5.1)

project(MNNLoader C CXX)

# Print debug current flag
message(STATUS "C flags: ${CMAKE_C_FLAGS}")
message(STATUS "CXX flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "C Debug flags: ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "CXX Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "C Release flags: ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "CXX Release flags: ${CMAKE_CXX_FLAGS_RELEASE}")


# ----- This is some option flag we can be define
option(TESTING "Build tests" OFF)

if (TESTING)
    find_package(GTest CONFIG REQUIRED)
endif ()

# ---------------------------
# Set default variable
# MNNLoader define version
set(VERSION_STRING 0.0.1)
set(SOVERSION_STRING 0.0.1)

# root dir
set(MNNLOADER_ROOT_DIR ${CMAKE_SOURCE_DIR})
# set default build dir

if (NOT DEFINED BUILD_MNNLOADER_DIR)
    set(BUILD_MNNLOADER_DIR ${MNNLOADER_ROOT_DIR}/.build/MNNLoader)
endif ()
message(STATUS "BUILD_MNNLOADER_DIR: ${BUILD_MNNLOADER_DIR}")
# Set source file dir
set(PROJECT.Path ${CMAKE_SOURCE_DIR}/src)
set(PROJECT.Source_DIR
    ${PROJECT.Path}
)

set(MNNLoader_LIB "mnnloader")
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_MNNLOADER_DIR}/lib" )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_MNNLOADER_DIR}/lib" )

SET(TESTAPP MNNExample )

# -----------------------------------
# Include external plugin need
include(ExternalProject)
include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/include
)


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
add_mnnloader_library(${VERSION_STRING} ${SOVERSION_STRING})
if (NEED_BUILD_MNN)
	# in case we need to build MNN library. Just waiting
	add_dependencies(${MNNLoader_LIB} MNN)
endif()
# -------------------------------------------
# Combine binary for execute example

link_directories(${BUILD_MNNLOADER_DIR}/lib)
add_mnnloader_executable("${TESTAPP}")

# install example binary to bin
install(TARGETS ${TESTAPP} DESTINATION "${BUILD_MNNLOADER_DIR}/bin")
