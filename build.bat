@echo off
set PATH=%PATH%;C:\android-ndk
ndk-build NDK_PROJECT_PATH=. NDK_APP_LIBS_OUT=out NDK_APPLICATION_MK=Application.mk
pause