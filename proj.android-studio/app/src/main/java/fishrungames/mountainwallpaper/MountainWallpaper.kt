/*
 * Copyright 2014 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package fishrungames.mountainwallpaper

import android.content.*
import android.content.pm.ApplicationInfo
import android.content.pm.PackageManager
import android.preference.PreferenceManager
import android.view.GestureDetector
import android.view.MotionEvent
import android.view.SurfaceHolder
import fishrungames.salmonengineandroid.EngineWrapper
import android.content.SharedPreferences



class MountainWallpaper : GLWallpaperService(), SharedPreferences.OnSharedPreferenceChangeListener {

    init
    {
        EngineWrapper.LoadSalmonEngineLibrary();
        //JniWrapper.LoadLibrary();
    }

    override fun onCreate() {
        super.onCreate()

        PreferenceManager.getDefaultSharedPreferences(this).registerOnSharedPreferenceChangeListener(this)

        EngineWrapper.SetActivityInstance(this)
        EngineWrapper.SetupEnviroment()

        var apkFilePath: String? = null
        var appInfo: ApplicationInfo? = null
        val packMgmr = this.packageManager
        try {
            appInfo = packMgmr.getApplicationInfo("fishrungames.mountainwallpaper", 0)
        } catch (e: PackageManager.NameNotFoundException) {
            e.printStackTrace()
            throw RuntimeException("Unable to locate assets, aborting...")
        }

        apkFilePath = appInfo!!.sourceDir

        EngineWrapper.SetupApkFilePath(apkFilePath)

        val prefs = PreferenceManager.getDefaultSharedPreferences(this)
        JniWrapper.SetTimeOfDayPref(Integer.parseInt(prefs.getString("Timeofday", "0")!!))
        JniWrapper.SetSnowPref(prefs.getBoolean("Snow", false))
    }

    override fun onStartCommand(intent: Intent, flags: Int, startId: Int): Int {
        return START_STICKY
    }

    override fun onCreateEngine(): Engine {
        return MuzeiWallpaperEngine()
    }

    override fun onSharedPreferenceChanged(sharedPreferences: SharedPreferences, key: String) {

        if (key.compareTo("Timeofday", ignoreCase = true) == 0) {
            val time = Integer.parseInt(sharedPreferences.getString("Timeofday", "0")!!)
            JniWrapper.SetTimeOfDayPref(time)
        }

        if (key.compareTo("Snow", ignoreCase = true) == 0) {

            JniWrapper.SetSnowPref(sharedPreferences.getBoolean("Snow", false))

        }
    }

    inner class MuzeiWallpaperEngine : GLWallpaperService.GLEngine()
    {

        private lateinit var renderer: WallpaperRenderer

        private val gestureListener = object : GestureDetector.SimpleOnGestureListener() {
            override fun onDown(e: MotionEvent): Boolean {
                return true
            }
        }
        private val gestureDetector: GestureDetector = GestureDetector(this@MountainWallpaper,
                gestureListener)

        override fun onCreate(surfaceHolder: SurfaceHolder) {
            super.onCreate(surfaceHolder)

            setEGLContextClientVersion(2)
            setRenderer(WallpaperRenderer())
            renderMode = RENDERMODE_CONTINUOUSLY
            requestRender()

            // Use the MuzeiWallpaperService's lifecycle to wait for the user to unlock

            setTouchEventsEnabled(true)
            setOffsetNotificationsEnabled(true)
        }

        override fun onOffsetsChanged(
                xOffset: Float,
                yOffset: Float,
                xOffsetStep: Float,
                yOffsetStep: Float,
                xPixelOffset: Int,
                yPixelOffset: Int
        ) {
            super.onOffsetsChanged(xOffset, yOffset, xOffsetStep, yOffsetStep, xPixelOffset,
                    yPixelOffset)

            JniWrapper.SetOffset(xOffset, 0.0f)
        }

        override fun onTouchEvent(event: MotionEvent) {
            super.onTouchEvent(event)
            gestureDetector.onTouchEvent(event)
        }
    }
}
