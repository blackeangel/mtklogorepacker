LOCAL_PATH := $(call my-dir)

##############################################################################
# libz
##############################################################################
include $(CLEAR_VARS)

LOCAL_MODULE := libz

LOCAL_CFLAGS := \
    -DHAVE_HIDDEN \
    -DZLIB_CONST

LOCAL_C_INCLUDES := \
    src/zlib  \

LOCAL_SRC_FILES += \
    zlib/adler32.c \
    zlib/compress.c \
    zlib/crc32.c \
    zlib/deflate.c \
    zlib/gzclose.c \
    zlib/gzlib.c \
    zlib/gzread.c \
    zlib/gzwrite.c \
    zlib/infback.c \
    zlib/inffast.c \
    zlib/inflate.c \
    zlib/inftrees.c \
    zlib/trees.c \
    zlib/uncompr.c \
    zlib/zutil.c

include $(BUILD_STATIC_LIBRARY)

##############################################################################
# libpng
##############################################################################
include $(CLEAR_VARS)

LOCAL_MODULE := libpng

LOCAL_CFLAGS := \
    -DHAVE_HIDDEN \
    -DZLIB_CONST

LOCAL_SRC_FILES += \
libpng/png.c \
libpng/pngerror.c \
libpng/pngget.c \
libpng/pngmem.c \
libpng/pngpread.c \
libpng/pngread.c \
libpng/pngrio.c \
libpng/pngrtran.c \
libpng/pngrutil.c \
libpng/pngset.c \
libpng/pngtrans.c \
libpng/pngwio.c \
libpng/pngwrite.c \
libpng/pngwtran.c \
libpng/pngwutil.c \

include $(BUILD_STATIC_LIBRARY)

#####################################################################################
#logo
#####################################################################################

include $(CLEAR_VARS)

LOCAL_MODULE := logo


LOCAL_CXXFLAGS := -fexceptions -std=c++2a -O2

LOCAL_LDFLAGS := -fPIE -static -ldl

LOCAL_SRC_FILES := $(wildcard src/*.cpp) \
					$(wildcard src/*.cxx)

LOCAL_STATIC_LIBRARIES := \
z \
png

include $(BUILD_EXECUTABLE)

