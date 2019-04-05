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

JNIEXPORT void JNICALL Java_fishrungames_reflectedmountain_JniWrapper_SetTimeOfDayPref(JNIEnv * env, jobject obj, jint timeofday)
{
	TimeOfDayPref = timeofday;
}
JNIEXPORT void JNICALL Java_fishrungames_reflectedmountain_JniWrapper_SetSnowPref(JNIEnv * env, jobject obj, jboolean snow)
{
	SnowPref = snow;
}

/*
namespace SE {
    int testSomethingOut();
    std::string testSomethingOut2(int v);
}*/

JNIEXPORT void JNICALL Java_fishrungames_reflectedmountain_JniWrapper_Init(JNIEnv * env, jobject obj,  jint width, jint height)
{
/*
    __android_log_print(ANDROID_LOG_VERBOSE, "SalmonWallpaper", "FRG Java_fishrungames_mountainwallpaper_JniWrapper_Init-");

    auto b = testSomethingOut2(99);

    __android_log_print(ANDROID_LOG_VERBOSE, "SalmonWallpaper", "FRG Java_fishrungames_mountainwallpaper_JniWrapper_Init- -- %s", b.c_str());

    GetConsole() << "JniWrapper_Init go!\n";

    __android_log_print(ANDROID_LOG_VERBOSE, "SalmonWallpaper", "FRG Java_fishrungames_mountainwallpaper_JniWrapper_Init-- 2");
*/
    auto app = JniInitApp<TAndroidApplication>(width, height, 800.f, 480.f);

    //__android_log_print(ANDROID_LOG_VERBOSE, "SalmonWallpaper", "FRG Java_fishrungames_mountainwallpaper_JniWrapper_Init-- 3");

    //GetConsole() << "JniWrapper_Init step 1!\n";

    if (app)
    {
        Application = app;
    }

    //GetConsole() << "JniWrapper_Init step 2!\n";

    Application->SkyTexShift = lastOffsetX*0.002f;

    //GetConsole() << "JniWrapper_Init step 3!\n";
}


JNIEXPORT void JNICALL Java_fishrungames_reflectedmountain_JniWrapper_SetOffset(JNIEnv * env, jobject obj, jfloat offsetX, jfloat offsetY)
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
