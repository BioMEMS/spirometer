package com.davis.AsthmaAppTablet;

import android.app.Activity;
import android.content.res.Resources;
import android.view.View;

public class AccessoryController extends Activity{

	protected DemoKitActivity mHostActivity;

	public AccessoryController(DemoKitActivity activity) {
		mHostActivity = activity;
	}

	public View findViewById(int id) {
		return mHostActivity.findViewById(id);
	}

	public Resources getResources() {
		return mHostActivity.getResources();
	}

	void accessoryAttached() {
		onAccesssoryAttached();
	}

	protected void onAccesssoryAttached() {
	}

}