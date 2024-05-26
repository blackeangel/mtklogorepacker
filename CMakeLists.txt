cmake_minimum_required(VERSION 3.20)

project(logo)
set(CMAKE_CXX_STANDARD 20)

set(DEFAULT_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")


set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out)

include(GNUInstallDirs)

include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/src)

# Section: zlib
set(ZLIB_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/zlib/adler32.c
${CMAKE_CURRENT_SOURCE_DIR}/zlib/compress.c
${CMAKE_CURRENT_SOURCE_DIR}/zlib/crc32.c
${CMAKE_CURRENT_SOURCE_DIR}/zlib/deflate.c
${CMAKE_CURRENT_SOURCE_DIR}/zlib/gzclose.c
${CMAKE_CURRENT_SOURCE_DIR}/zlib/gzlib.c
${CMAKE_CURRENT_SOURCE_DIR}//zlib/gzread.c
${CMAKE_CURRENT_SOURCE_DIR}/zlib/gzwrite.c
${CMAKE_CURRENT_SOURCE_DIR}/zlib/infback.c
${CMAKE_CURRENT_SOURCE_DIR}/zlib/inffast.c
${CMAKE_CURRENT_SOURCE_DIR}/zlib/inflate.c
${CMAKE_CURRENT_SOURCE_DIR}/zlib/inftrees.c
${CMAKE_CURRENT_SOURCE_DIR}/zlib/trees.c
${CMAKE_CURRENT_SOURCE_DIR}/zlib/uncompr.c
${CMAKE_CURRENT_SOURCE_DIR}/zlib/zutil.c
)
add_library(z STATIC ${ZLIB_SRC_FILES})

# Section: libpng
set(LIBPNG_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/libpng/png.c
${CMAKE_CURRENT_SOURCE_DIR}/libpng/pngerror.c
${CMAKE_CURRENT_SOURCE_DIR}/libpng/pngget.c
${CMAKE_CURRENT_SOURCE_DIR}/libpng/pngmem.c
${CMAKE_CURRENT_SOURCE_DIR}/libpng/pngpread.c
${CMAKE_CURRENT_SOURCE_DIR}/libpng/pngread.c
${CMAKE_CURRENT_SOURCE_DIR}/libpng/pngrio.c
${CMAKE_CURRENT_SOURCE_DIR}/libpng/pngrtran.c
${CMAKE_CURRENT_SOURCE_DIR}/libpng/pngrutil.c
${CMAKE_CURRENT_SOURCE_DIR}/libpng/pngset.c
${CMAKE_CURRENT_SOURCE_DIR}/libpng/pngtrans.c
${CMAKE_CURRENT_SOURCE_DIR}/libpng/pngwio.c
${CMAKE_CURRENT_SOURCE_DIR}/libpng/pngwrite.c
${CMAKE_CURRENT_SOURCE_DIR}/libpng/pngwtran.c
${CMAKE_CURRENT_SOURCE_DIR}/libpng/pngwutil.c
)
add_library(png STATIC ${LIBPNG_SRC_FILES})

# Section: minizip
set(LIBMINIZIP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/minizip/ioapi.c
${CMAKE_CURRENT_SOURCE_DIR}/minizip/iowin32.c
${CMAKE_CURRENT_SOURCE_DIR}/minizip/miniunz.c
${CMAKE_CURRENT_SOURCE_DIR}/minizip/minizip.c
${CMAKE_CURRENT_SOURCE_DIR}/minizip/mztools.c
${CMAKE_CURRENT_SOURCE_DIR}/minizip/unzip.c
${CMAKE_CURRENT_SOURCE_DIR}/minizip/zip.c
)

add_library(minizip STATIC ${LIBMINIZIP_SRC_FILES})

target_link_libraries(png z minizip)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

# Section: main
set(LOGO_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/unpack.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/pack.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/images.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
)
add_executable(logo ${LOGO_SRC_FILES})
target_link_libraries(logo png z)
