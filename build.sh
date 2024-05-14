#!/bin/bash
home_dir=$(dirname $HOME)
path_ndk="$home_dir/android-ndk"
path_sdk="$home_dir/android-sdk"
PATH=$path_ndk:$path_sdk:$PATH

check_ndk() {
which ndk-build >/dev/null 2>&1
if [ $? != 0 ]; then
	echo "Не удается найти ndk-build в $PATH.Убедитесь, что каталог Android NDK включен в вашу переменную \$PATH"
    exit 1
fi
}

ndk-build NDK_PROJECT_PATH=. NDK_APP_LIBS_OUT=out NDK_APPLICATION_MK=Application.mk $@
name_file="bin_utils_"$(date +%Y%m%d%H%M)
cd ./out/armeabi-v7a
../../zip -9D /sdcard/$name_file.zip ./*
#tar -czRf /sdcard/$name_file.tgz ./
#gzip ./out/armeabi-v7a/bin_utils /sdcard/$name_file.gz
