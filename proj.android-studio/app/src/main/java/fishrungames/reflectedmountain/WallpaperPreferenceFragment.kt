package fishrungames.reflectedmountain

import android.support.v7.preference.PreferenceFragmentCompat
import android.os.Bundle

class WallpaperPreferenceFragment : PreferenceFragmentCompat()
{
    override fun onCreatePreferences(savedInstanceState: Bundle?, rootKey: String?) {
        setPreferencesFromResource(R.xml.preferences, rootKey)
    }
}