# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.


add_definitions(-DTARGET_ANDROID)

set(BOOST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../boost_1_63_0)

set(BOOST_GIL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/boost-gil-extension)

set(ZIP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/julienr-libzip-android/jni)

set(LIBPNG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/libpng_1.4.1_android)

set(LIBJPEG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/jpeg-9")


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../engine)

include_directories(${BOOST_PATH})
include_directories(${LIBPNG_PATH})
include_directories(${LIBJPEG_PATH})
include_directories(${ZIP_PATH})

include_directories(${BOOST_GIL_PATH})

add_library( engine
             SHARED
             IMPORTED )
			 
set_target_properties( # Specifies the target library.
                       engine

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${CMAKE_CURRENT_SOURCE_DIR}/build/intermediates/exploded-aar/proj.android-studio/SalmonEngineAndroid/unspecified/jni/${ANDROID_ABI}/libengine.so )
					   



add_library( # Sets the name of the library.
             MountainWallpaper

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             ${CMAKE_CURRENT_SOURCE_DIR}/../../game/android_api.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/../../game/main_code.cpp
             )
			 
			 

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )
			  
find_library( # Sets the name of the path variable.
              GLESv2-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              GLESv2 )


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       MountainWallpaper

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} ${GLESv2-lib} engine )
