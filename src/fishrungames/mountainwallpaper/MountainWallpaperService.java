package fishrungames.mountainwallpaper;


import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;

import android.net.ConnectivityManager;

import android.os.Handler;
import android.preference.PreferenceManager;

import android.view.MotionEvent;
import android.view.SurfaceHolder;

//import com.seb.SLWP.SLWP.GlEngine.DownloadTask;

import fishrungames.engine.EngineWrapper;

public class MountainWallpaperService extends GLWallpaperService implements
		OnSharedPreferenceChangeListener {
	/*
	 * (non-Javadoc)
	 * 
	 * @see android.app.Service#onLowMemory()
	 */
	static
	{
		EngineWrapper.LoadSalmonEngineLibrary();
		JniWrapper.LoadLibrary();
	}


	public static final long SLEEPTIME = 1000 * 60 * 30; // 30 minutes
	private GlEngine mGle;
	
	public static Context mContext;
	public static boolean ShowClouds;
	public static boolean TouchRot;
	
	public static int Bg;
	public static boolean Usebg;
	public static int Tex;
	
	static long Synctime;
	
	public boolean Slidedir;
	public boolean Slideplanet;
	public boolean Syncrot;
	public static boolean Randomtex = true;
	public static boolean visible = false;
	
	public static boolean useCropper = true;
	public static int Cropaspect;
	public static boolean loading = false;
	
	public static final Handler mHandler = new Handler();
	public static final int RNDMAP = -1;
	public static boolean destroyed;
	public static String bgfile;
	public ConnectivityManager cm;
	public boolean needresume;
	public boolean fstart;
	public static String[] randlist;
	public int curtexidx = -99;

	static final String ACTION_FOREGROUND = "fishrungames.mountainwallpaper.FOREGROUND";
	static final String ACTION_BACKGROUND = "fishrungames.mountainwallpaper.BACKGROUND";
		


	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		super.onCreate();
		
		
		PreferenceManager.getDefaultSharedPreferences(this).registerOnSharedPreferenceChangeListener(this);

		EngineWrapper.SetActivityInstance(this);
		EngineWrapper.SetupEnviroment();
		
		String apkFilePath = null;
		ApplicationInfo appInfo = null;
		PackageManager packMgmr = this.getPackageManager();
		try {
		        appInfo = packMgmr.getApplicationInfo("fishrungames.mountainwallpaper", 0);
		    } catch (NameNotFoundException e) {
		 
	
	
		 e.printStackTrace();
		throw new RuntimeException("Unable to locate assets, aborting...");
		    }
		apkFilePath = appInfo.sourceDir;
		
		EngineWrapper.SetupApkFilePath(apkFilePath);
		
		Init();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		// handleCommand(intent);
		// We want this service to continue running until it is explicitly
		// stopped, so return sticky.
		return START_STICKY;
	}

	private void Init() {
		try {
			mContext = this;
			cm = (ConnectivityManager) mContext
					.getSystemService(Context.CONNECTIVITY_SERVICE);
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
	}

	@Override
	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences,
			String key) {
		
		if (key.compareToIgnoreCase("Timeofday") == 0)
		{
			int time = Integer.parseInt(sharedPreferences.getString("Timeofday", "0"));
			JniWrapper.SetTimeOfDayPref(time);
		}
		
		if (key.compareToIgnoreCase("Snow") == 0) {
			
			JniWrapper.SetSnowPref(sharedPreferences.getBoolean("Snow", false));

		}
	}

	@Override
	public Engine onCreateEngine() {
		if (mGle != null) {
			mGle = null;
		}
		try {
			
			mGle = new GlEngine();
			
		} catch (Exception e) {
			return null;
		}

		return mGle;
	}

	class GlEngine extends GLEngine {

		public Handler mHandler = new Handler();
		
		long NOW;
		
		@Override
		public void onCreate(SurfaceHolder surfaceHolder) {
			super.onCreate(surfaceHolder);
			try {
				this.setTouchEventsEnabled(true);
				fstart = true;
				setRenderer();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		@Override
		public void onTouchEvent(MotionEvent e) {
			
			return;
		}

		@Override
		public void onOffsetsChanged(float xOffset, float yOffset,
				float xOffsetStep, float yOffsetStep, int xPixelOffset,
				int yPixelOffset) {
			
			super.onOffsetsChanged(xOffset, yOffset, xOffsetStep, yOffsetStep,
					xPixelOffset, yPixelOffset);
			
			JniWrapper.SetOffset(xPixelOffset*xOffsetStep, yPixelOffset*yOffsetStep);
			
		}

		@Override
		public void onVisibilityChanged(boolean visible) {
			// TODO Auto-generated method stub
			super.onVisibilityChanged(visible);
			MountainWallpaperService.visible = visible;
			if (visible) {

			} else {

			}
		}

		@Override
		public void onPause() {
			super.onPause();
		}

		@Override
		public void onResume() {
			
			super.onResume();
		}

		@Override
		public void onSurfaceChanged(SurfaceHolder holder, int format,
				int width, int height) {
			// TODO Auto-generated method stub
			super.onSurfaceChanged(holder, format, width, height);
			fstart = true;
		}

	}

}