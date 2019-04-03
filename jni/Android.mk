# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


LPATH := $(call my-dir)

ENGINE_PATH = $(SalmonEnginePathCygwin)

BOOST_PATH = $(LibsPathCygwin)/boost_1_52_0

OGG_PATH = $(LibsPathCygwin)/libogg-1.3.0

VORBIS_PATH = $(LibsPathCygwin)/libvorbis-1.3.2

SQUIRREL_PATH = $(LibsPathCygwin)/sqplus

LIBPNG_PATH = $(LibsPathCygwin)/libpng_1.4.1_android

ZIP_PATH = $(LibsPathCygwin)/julienr-libzip-android/jni

#====== ENGINE AND LIBS =====================

include $(ENGINE_PATH)/Android_Salmon_Engine.mk

#================= THE GAME =======================


LOCAL_PATH:= $(LPATH)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -DTARGET_ANDROID -DNOSOUND -std=gnu++11 --std=c++11
LOCAL_STATIC_LIBRARIES := boost
LOCAL_STATIC_LIBRARIES += squirrel
LOCAL_STATIC_LIBRARIES += png_lib
LOCAL_STATIC_LIBRARIES += zip
LOCAL_SHARED_LIBRARIES := SalmonEngine
LOCAL_SHARED_LIBRARIES += gnustl_shared
LOCAL_C_INCLUDES := $(ENGINE_PATH)
LOCAL_C_INCLUDES += $(BOOST_PATH)
LOCAL_C_INCLUDES += $(VORBIS_PATH)/include
LOCAL_C_INCLUDES += $(VORBIS_PATH)/include/vorbis
LOCAL_C_INCLUDES += $(VORBIS_PATH)/lib
LOCAL_C_INCLUDES += $(VORBIS_PATH)/lib/books
LOCAL_C_INCLUDES += $(VORBIS_PATH)/lib/modes
LOCAL_C_INCLUDES += $(OGG_PATH)/include
LOCAL_C_INCLUDES += $(OGG_PATH)/include/ogg
LOCAL_C_INCLUDES += $(SQUIRREL_PATH)/include
LOCAL_C_INCLUDES += $(SQUIRREL_PATH)/sqplus
LOCAL_C_INCLUDES += $(SQUIRREL_PATH)/squirrel
LOCAL_C_INCLUDES += $(SQUIRREL_PATH)/sqstdlib
LOCAL_C_INCLUDES += $(LIBPNG_PATH)
LOCAL_C_INCLUDES += $(ZIP_PATH)
LOCAL_MODULE    := SalmonWallpaper
LOCAL_SRC_FILES := main_code.cpp
LOCAL_SRC_FILES += android_api.cpp

LOCAL_LDLIBS    := -lGLESv2
LOCAL_LDLIBS    += -llog -Wl

#debug
#LOCAL_CFLAGS += -g -ggdb -O0
#LOCAL_LDLIBS    += -g -ggdb

include $(BUILD_SHARED_LIBRARY)
