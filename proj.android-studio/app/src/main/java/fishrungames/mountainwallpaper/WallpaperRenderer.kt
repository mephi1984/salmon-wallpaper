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

import android.opengl.GLES20
import android.opengl.GLSurfaceView
import android.util.Log
import fishrungames.salmonengineandroid.EngineWrapper
import java.util.*
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class WallpaperRenderer() : GLSurfaceView.Renderer {

    private var surfaceCreated: Boolean = false
    private var lastTimeStamp: Long = 0

    override fun onSurfaceCreated(unused: GL10, config: EGLConfig) {
        surfaceCreated = true
    }

    override fun onSurfaceChanged(unused: GL10, width: Int, height: Int) {

        Log.v("WallpaperRenderer", "onSurfaceChanged")
        try {
            JniWrapper.Init(width, height)
        }
        catch (t: Throwable) {

            t.printStackTrace()
            System.out.println("Exception occurred");
        }
        Log.v("WallpaperRenderer", "onSurfaceChanged2")
    }

    override fun onDrawFrame(unused: GL10) {
        val currentTimeStamp = Calendar.getInstance().timeInMillis
        if (lastTimeStamp == 0L)
        {
            lastTimeStamp = currentTimeStamp
        }

        Log.v("WallpaperRenderer", "onDrawFrame")

        EngineWrapper.Update(currentTimeStamp - lastTimeStamp)

        lastTimeStamp = currentTimeStamp
    }
}
