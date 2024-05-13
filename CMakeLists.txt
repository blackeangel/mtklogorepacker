cmake_minimum_required(VERSION 3.20)
project(logo)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out)

include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/zlib
        ${PROJECT_SOURCE_DIR}/libpng
        ${PROJECT_SOURCE_DIR}/src
)
file(GLOB SRCS
        "${PROJECT_SOURCE_DIR}/include/*"
        "${PROJECT_SOURCE_DIR}/zlib/*"
        "${PROJECT_SOURCE_DIR}/libpng/*"
        "${PROJECT_SOURCE_DIR}/src/*"
)
add_executable(logo ${SRCS})