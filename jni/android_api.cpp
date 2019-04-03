#include "android_api.h"

#include "main_code.h"
#include "boost\thread.hpp"

TAndroidApplication* Application = NULL;


float lastOffsetX = 0.5f;

const float CONST_ANGLE_SHIFT = 45.f;

int TimeOfDayPref = 0;
bool SnowPref = false;

bool OffsetChanged = false;

extern TPanoramicCamera mCamera;
extern TPanoramicCamera mInvCamera;

JNIEXPORT void JNICALL Java_fishrungames_mountainwallpaper_JniWrapper_SetTimeOfDayPref(JNIEnv * env, jobject obj, jint timeofday)
{
	TimeOfDayPref = timeofday;
}
JNIEXPORT void JNICALL Java_fishrungames_mountainwallpaper_JniWrapper_SetSnowPref(JNIEnv * env, jobject obj, jboolean snow)
{
	SnowPref = snow;
}

JNIEXPORT void JNICALL Java_fishrungames_mountainwallpaper_JniWrapper_Init(JNIEnv * env, jobject obj,  jint width, jint height)
{
    auto app = JniInitApp<TAndroidApplication>(width, height, 800.f, 480.f);

    if (app)
    {
        Application = app;
    }

    Application->SkyTexShift = lastOffsetX*0.002f;
}


JNIEXPORT void JNICALL Java_fishrungames_mountainwallpaper_JniWrapper_SetOffset(JNIEnv * env, jobject obj, jfloat offsetX, jfloat offsetY)
{

	if (Renderer != NULL && Application != nullptr)
	{
		OffsetChanged = true;
		Application->SkyTexShift = offsetX;
		lastOffsetX = offsetX;

        mCamera.SetAlpha((-lastOffsetX) * pi);
        mInvCamera.SetAlpha((-lastOffsetX) * pi);
	}
}
