#add_subdirectory(application)
file(GLOB FILELOADER_HEADER "../include/*.h*")
file(GLOB FILELOADER_SRCS "*.cpp")
FILE(GLOB MNN_LIBS "${MNN_LIBRARY_DIR}/*")
add_library(fileloader
	${FILELOADER_HEADER}
    ${FILELOADER_SRCS}
    )
target_link_libraries(fileloader)
file(INSTALL ${FILELOADER_HEADER} DESTINATION ${CMAKE_BINARY_DIR}/FileLoader/include)
include_directories(${CMAKE_BINARY_DIR}/FileLoader/include)
link_directories(${CMAKE_BINARY_DIR}/FileLoader/lib)