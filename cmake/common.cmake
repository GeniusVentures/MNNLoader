function(add_mnnloader_library version soversion)
   # 1. Glob header files
   file(GLOB MMLOADER_HEADER "${CMAKE_SOURCE_DIR}/include/*.hpp")

   # 2. Glob sources files
   file(GLOB MNNLOADER_SRCS "${CMAKE_SOURCE_DIR}/src/*.cpp")

   set(MNNLOADER_OBJECT_LINK "")
   set(MNNLOADER_TARGETS "")

   FILE(GLOB MNN_LIBS "${MNN_LIBRARY_DIR}/*")
   list(APPEND MNNLOADER_OBJECT_LINK ${MNNLOADER_OBJECT_LINK} ${MNN_LIBS} ${MNNLOADER_SRCS})
   list(APPEND MNNLOADER_TARGETS mnnloader)

   ## 3. library
   # TODO: Add flag for build SHARED or STATIC library. Currently just default is static (GNUS is not using shared libraries)
   add_library(${MNNLoader_LIB} ${MNNLOADER_SRCS})
   target_link_libraries(${MNNLoader_LIB} ${MNN_LIBS})

   message(STATUS "Installing MNNLOADER Headers ...")
   file(INSTALL ${MMLOADER_HEADER} DESTINATION ${BUILD_MNNLOADER_DIR}/include)
   message(STATUS ">>> Added Library: MNNLoader!")

endfunction()


function(add_mnnloader_executable executable_name)
    add_executable(${executable_name} ${executable_name}.cpp)
    target_link_libraries(${executable_name} ${MNNLoader_LIB} ${MNN_LIBS})
endfunction()