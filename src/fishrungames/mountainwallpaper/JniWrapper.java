package fishrungames.mountainwallpaper;

public class JniWrapper
{

	static
	{
		System.loadLibrary("SalmonWallpaper");
	}
	
	public static void LoadLibrary()
	{
		//To force loading libraries
	}
	
	public static native void SetTimeOfDayPref(int timeofday);
	
	public static native void SetSnowPref(boolean snow);
	
	
	public static native void Init(int width, int height);
	
	public static native void SetOffset(float offsetX, float offsetY);
	
	public static native void SetOrientation(int isLandscape);
	
}
