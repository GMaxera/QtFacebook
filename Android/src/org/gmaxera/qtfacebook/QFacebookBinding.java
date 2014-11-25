package org.gmaxera.qtfacebook;

import com.facebook.*;
import com.facebook.model.*;
import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.content.Intent;
import java.util.ArrayList;

/*! Java class for bind the C++ method of QFacebook to the
 *  java implementation using the native Facebook SDK for Android
 *
 *  This class is a singleton because it's a binding of a singleton C++ object
 *  All the methods are static, and they use the private singleton instance
 */
public class QFacebookBinding implements Session.StatusCallback {
	// Singleton instance created as soon as possibile
	private static final QFacebookBinding m_instance = new QFacebookBinding();
	// Activity of which this QFacebookBinding is associated
	private Activity activity = null;
	// Used by Facebook SDK for handling UI transitions
	private UiLifecycleHelper uiLifecycleHelper = null;
	//! subset of requestPermissions that only allow reading from Facebook
	ArrayList<String> readPermissions = new ArrayList<String>();
	//! subset of requestPermissions that allow writing to Facebook
	ArrayList<String> writePermissions = new ArrayList<String>();

	// private constructor
	private QFacebookBinding() {
		// nothing to do here
	}

	//! Clear the readPermissions list
	static public void readPermissionsClear() {
		m_instance.readPermissions.clear();
	}
	//! Add a permission to the read permissions list
	static public void readPermissionsAdd( String permission ) {
		m_instance.readPermissions.add( permission );
	}
	//! Clear the writePermissions list
	static public void writePermissionsClear() {
		m_instance.writePermissions.clear();
	}
	//! Add a permission to the write permissions list
	static public void writePermissionsAdd( String permission ) {
		m_instance.writePermissions.add( permission );
	}

	//! This has to be called inside the onCreate of Activity
	static public void onCreate(Activity activity, Bundle savedInstanceState) {
		m_instance.activity = activity;
		if ( m_instance.uiLifecycleHelper == null ) {
			m_instance.uiLifecycleHelper = new UiLifecycleHelper(m_instance.activity, m_instance);
		}
		m_instance.uiLifecycleHelper.onCreate(savedInstanceState);
	}

	//! This has to be called inside the onResume of Activity
	static public void onResume() {
		m_instance.uiLifecycleHelper.onResume();
	}

	//! This has to be called inside the onDestroy of Activity
	static public void onDestroy() {
		m_instance.uiLifecycleHelper.onDestroy();
	}

	//! This has to be called inside the onActivityResult of Activity
	static public void onActivityResult(int requestCode, int resultCode, Intent data) {
		m_instance.uiLifecycleHelper.onActivityResult(requestCode, resultCode, data);
	}

	// Perform the login into Facebook
	static public void login() {
		Session.OpenRequest request = new Session.OpenRequest(m_instance.activity);
		request.setPermissions( m_instance.readPermissions );
		if (Session.getActiveSession() == null || Session.getActiveSession().isClosed()) {
			Session session = new Session.Builder( m_instance.activity.getApplicationContext() ).build();
			Session.setActiveSession(session);
		}
		Session.getActiveSession().openForRead( request );
	}

	// The Session.StatusCallback method
	@Override
	public void call(Session psession, SessionState state, Exception exception) {
		// check if there was an exception
		if (exception != null) {
			if (exception instanceof FacebookOperationCanceledException &&
				!SessionState.OPENED_TOKEN_UPDATED.equals(state)) {
				// HERE THE USER DID NOT ACCEPT SOMETHING
			} else {
				exception.printStackTrace();
			}
		}
		// check the current state and acts accordlying
		switch (state) {
		case CLOSED:
			Log.i("QFacebook", "Facebook State is CLOSED");
		break;
		case CLOSED_LOGIN_FAILED:
			Log.i("QFacebook", "Facebook State is CLOSED_LOGIN_FAILED");
		break;
		case CREATED:
			Log.i("QFacebook", "Facebook State is CREATED");
		break;
		case CREATED_TOKEN_LOADED:
			Log.i("QFacebook", "Facebook State is CREATED_TOKEN_LOADED");
		break;
		case OPENING:
			Log.i("QFacebook", "Facebook State is OPENING");
		break;
		case OPENED:
			Log.i("QFacebook", "Facebook State is OPENED");
		break;
		case OPENED_TOKEN_UPDATED:
			Log.i("QFacebook", "Facebook State is OPENED_TOKEN_UPDATED");
		break;
		}
	}

}
