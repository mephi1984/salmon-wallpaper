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

package fishrungames.reflectedmountain

import android.os.Bundle
import android.support.v4.app.FragmentActivity
import android.app.WallpaperManager
import android.view.View
import android.widget.Button
import android.content.Intent
import android.content.ComponentName

class WallpaperPreferences : FragmentActivity()
{
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.pref_activity)
        val buttonSetWallpaper = findViewById<View>(R.id.button_setWallpaper) as Button

        buttonSetWallpaper.setOnClickListener(object : View.OnClickListener{
            override fun onClick(arg0: View) {

                val component = ComponentName(packageName, "$packageName.MountainWallpaper")
                intent = Intent(WallpaperManager.ACTION_CHANGE_LIVE_WALLPAPER)
                intent.putExtra(WallpaperManager.EXTRA_LIVE_WALLPAPER_COMPONENT, component)
                startActivity(intent)

            }
        })
    }
}
