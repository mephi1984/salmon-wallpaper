#ifndef ANDROID_API_H_INCLUDED
#define ANDROID_API_H_INCLUDED

#include <jni.h>
#include <android/log.h>

#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "boost/shared_ptr.hpp"

#include "main_code.h"

using namespace SE;

extern "C" {

	JNIEXPORT void JNICALL Java_fishrungames_mountainwallpaper_JniWrapper_SetTimeOfDayPref(JNIEnv * env, jobject obj, jint timeofday);
	
	JNIEXPORT void JNICALL Java_fishrungames_mountainwallpaper_JniWrapper_SetSnowPref(JNIEnv * env, jobject obj, jboolean snow);
	
	JNIEXPORT void JNICALL Java_fishrungames_mountainwallpaper_JniWrapper_Init(JNIEnv * env, jobject obj,  jint width, jint height);
    
	JNIEXPORT void JNICALL Java_fishrungames_mountainwallpaper_JniWrapper_SetOffset(JNIEnv * env, jobject obj, jfloat offsetX, jfloat offsetY);

	};


#endif
