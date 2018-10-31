cmake_minimum_required(VERSION 3.4.1)

set(BOOST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../boost_1_67_0)
set(BOOST_GIL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/boost-gil-extension)
set(ZIP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/julienr-libzip-android/jni)
set(LIBPNG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/libpng_1.4.1_android)
set(LIBJPEG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/jpeg-9)
set(EIGEN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../eigen)
set(SOL2_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../sol2)
set(LUA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/lua-5.3.4/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../tes-engine)
include_directories(${BOOST_PATH})
include_directories(${EIGEN_PATH})
include_directories(${SOL2_PATH})
include_directories(${LUA_PATH})
include_directories(${LIBPNG_PATH})
include_directories(${LIBJPEG_PATH})
include_directories(${ZIP_PATH})
include_directories(${BOOST_GIL_PATH})

add_definitions(-DTARGET_ANDROID)

add_library(engine SHARED IMPORTED

        )

set_target_properties(engine PROPERTIES IMPORTED_LOCATION

        ${CMAKE_CURRENT_SOURCE_DIR}/../../../tes-engine/SalmonEngineAndroid/app/build/intermediates/cmake/debug/obj/${ANDROID_ABI}/libengine.so

        )



add_library( # Sets the name of the library.
             MountainWallpaper

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             ${CMAKE_CURRENT_SOURCE_DIR}/../../jni/android_api.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/../../jni/main_code.cpp
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
