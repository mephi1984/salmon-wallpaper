package fishrungames.reflectedmountain;

class JniWrapper
{
	static
	{
		System.loadLibrary("MountainWallpaper");
	}

	public static native void SetTimeOfDayPref(int timeofday);

	public static native void SetSnowPref(boolean snow);

	public static native void Init(int width, int height);

	public static native void SetOffset(float offsetX, float offsetY);

}
