package fishrungames.mountainwallpaper;

import fishrungames.mountainwallpaper.R;
import fishrungames.mountainwallpaper.JniWrapper;


import android.content.Intent;

import android.net.Uri;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;

import android.preference.Preference.OnPreferenceChangeListener;


public class Prefs extends PreferenceActivity implements
		OnPreferenceChangeListener {

	
	private CheckBoxPreference snowPref;
	private ListPreference timeOfDayPref;
	

	public static Uri currImageURI;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.prefliste);
		addPreferencesFromResource(R.xml.preferences);
		
		
		snowPref = (CheckBoxPreference)this.getPreferenceManager().findPreference("Snow");
		timeOfDayPref = (ListPreference) findPreference("Timeofday");
		
		if (timeOfDayPref.getKey().compareToIgnoreCase("Timeofday") == 0)
		{
			JniWrapper.SetTimeOfDayPref(Integer.parseInt((String) timeOfDayPref.getValue()));
			
		}
		
		if (snowPref.getKey().compareToIgnoreCase("Snow") == 0) {
			if (!snowPref.isChecked())
			{
				JniWrapper.SetSnowPref(false);
			}
			else
			{
				JniWrapper.SetSnowPref(true);
			}
		}	

	}


	@Override
	public boolean onPreferenceChange(Preference preference, Object newValue) {
		
		return true;

	}


	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		
	}

}
