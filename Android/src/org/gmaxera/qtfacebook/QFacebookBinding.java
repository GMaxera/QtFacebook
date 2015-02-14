package org.gmaxera.qtfacebook;

import com.facebook.*;
import com.facebook.model.*;
import com.facebook.widget.*;
import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.content.Intent;
import java.util.ArrayList;
import java.util.List;
import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import java.lang.Thread;

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

	//! This has to be called inside the onSaveInstanceState of Activity
	static public void onSaveInstanceState(Bundle outState) {
		m_instance.uiLifecycleHelper.onSaveInstanceState(outState);
	}

	//! This has to be called inside the onPause of Activity
	static public void onPause() {
		m_instance.uiLifecycleHelper.onPause();
	}

	//! This has to be called inside the onDestroy of Activity
	static public void onDestroy() {
		m_instance.uiLifecycleHelper.onDestroy();
	}

	//! This has to be called inside the onActivityResult of Activity
	static public void onActivityResult(int requestCode, int resultCode, Intent data) {
		m_instance.uiLifecycleHelper.onActivityResult(requestCode, resultCode, data, new FacebookDialog.Callback() {
			@Override
			public void onError(FacebookDialog.PendingCall pendingCall, Exception error, Bundle data) {
				Log.e("Activity", String.format("Error: %s", error.toString()));
			}

			@Override
			public void onComplete(FacebookDialog.PendingCall pendingCall, Bundle data) {
				Log.i("Activity", "Success!");
			}
		});
	}

	// Check the activeSession and create is if needed
	static public void createSessionIfNeeded() {
		if (Session.getActiveSession() == null || Session.getActiveSession().isClosed()) {
			Log.i("QFacebook", "Facebook Creating a new Session");
			Session session = new Session.Builder( m_instance.activity.getApplicationContext() ).build();
			Session.setActiveSession(session);
		} else {
			Log.i("QFacebook", "Facebook Session still valid "+Session.getActiveSession().getState());
		}
	}

	// Perform the login into Facebook
	static public void login() {
		Session.OpenRequest request = new Session.OpenRequest(m_instance.activity);
		request.setPermissions( m_instance.readPermissions );
		createSessionIfNeeded();
		Session.getActiveSession().openForRead( request );
		Log.i("QFacebook", "Requesting Login to Facebook");
	}

	// Perform the logout and clear any token information
	static public void close() {
		createSessionIfNeeded();
		Session.getActiveSession().closeAndClearTokenInformation();
	}

	// Request the write permissions
	static public void requestPublishPermissions() {
		Session.NewPermissionsRequest request = new Session.NewPermissionsRequest(m_instance.activity, m_instance.writePermissions);
		createSessionIfNeeded();
		Session.getActiveSession().requestNewPublishPermissions( request );
	}

	// Publish a photo
	static public void publishPhoto( byte[] imgBytes, String message ) {
		Log.i("QFacebook", "Facebook start Publishing Photo");
		Bitmap photo = BitmapFactory.decodeByteArray( imgBytes, 0, imgBytes.length );
		createSessionIfNeeded();
		final Request request = Request.newUploadPhotoRequest(
			Session.getActiveSession(),
			photo,
			new Request.Callback() {
				// The Request.Callback method
				@Override
				public void onCompleted(Response response) {
					Log.i("QFacebook", "Publish Photo Request Completed");
					operationDone( "publishPhoto" );
					GraphObject graphObject = response.getGraphObject();
					FacebookRequestError error = response.getError();
					if ( graphObject != null ) {
						Log.i("QFacebook", "Graph Object exists: "+graphObject.getProperty(Response.SUCCESS_KEY));
					}
					if ( error != null ) {
						Log.i("QFacebook", "Response terminated with an Error");
					}
				}
			}
		);
		if ( !message.isEmpty() ) {
			Bundle params = request.getParameters();
			params.putString( "message", message );
		}
		//RequestAsyncTask task = new RequestAsyncTask(request);
		//task.execute();
		//Request.executeBatchAsync(request);
		//Request.executeAndWait(request);
		Thread thread = new Thread() {
			@Override
			public void run() {
				Request.executeAndWait(request);
			}
		};
		thread.start();
	}

	// Publish a link with a photo using Share Dialog. If the Share Dialog is not available
	// (e.g. because the user hasn't the Facebook app installed), falls back to using the
	// Feed Dialog. This function does not require the user to be logged into Facebook from
	// the app. linkName is the name of the link, link is the link url, imageUrl is the url
	// of the image associated wih the link.
	static public void publishLinkViaShareDialog( final String linkName, final String link, final String imageUrl ) {
		// Creating the session if it doesn't exist yet
		createSessionIfNeeded();

		// First of all checking if we can use the ShareDialog and using it if we can
		if (FacebookDialog.canPresentShareDialog(m_instance.activity.getApplicationContext(), FacebookDialog.ShareDialogFeature.SHARE_DIALOG)) {
			Log.i("QFacebook", "Publishing using Share Dialog");

			// Publish the post using the Share Dialog. We start the dialog from the UI thread
			m_instance.activity.runOnUiThread(new Runnable() {
				public void run() {
					FacebookDialog shareDialog = new FacebookDialog.ShareDialogBuilder(m_instance.activity)
						.setLink(link)
						.setName(linkName)
						.setPicture(imageUrl)
						.build();
					m_instance.uiLifecycleHelper.trackPendingDialogCall(shareDialog.present());
				}
			});
		} else {
			Log.i("QFacebook", "Publishing using Feed Dialog");

			// Falling back to using the Feed Dialog
			final Bundle params = new Bundle();
			params.putString("name", linkName);
			params.putString("link", link);
			params.putString("picture", imageUrl);

			// Starting the Feed Dialog from the UI thread
			m_instance.activity.runOnUiThread(new Runnable() {
				public void run() {
					WebDialog.FeedDialogBuilder feedDialogBuilder = new WebDialog.FeedDialogBuilder(m_instance.activity, Session.getActiveSession(), params);

					feedDialogBuilder.setOnCompleteListener(new WebDialog.OnCompleteListener() {
						@Override
						public void onComplete(Bundle values, FacebookException error) {
							if (error == null) {
								// When the story is posted, echo the success
								// and the post Id.
								final String postId = values.getString("post_id");
								if (postId != null) {
									Log.i("QFacebook", "Posted story, id: "+postId);
								} else {
									// User clicked the Cancel button
									Log.i("QFacebook", "Publish cancelled");
								}
							} else if (error instanceof FacebookOperationCanceledException) {
								// User clicked the "x" button
								Log.i("QFacebook", "Publish cancelled");
							} else {
								// Generic, ex: network error
								Log.e("QFacebook", "Error posting story");
							}
						}
					});

					WebDialog feedDialog = feedDialogBuilder.build();
					feedDialog.show();
				}
			});
		}
	}


	// The Session.StatusCallback method
	@Override
	public void call(Session session, SessionState state, Exception exception) {
		// check if there was an exception
		if (exception != null) {
			if (exception instanceof FacebookOperationCanceledException &&
				!SessionState.OPENED_TOKEN_UPDATED.equals(state)) {
				// HERE THE USER DID NOT ACCEPT SOMETHING
				Log.i("QFacebook", "The user did not accept something...");
			} else {
				exception.printStackTrace();
				Throwable cause = exception;
				System.err.println(exception.getMessage());
				while (cause.getCause() != null) {
					cause = cause.getCause();
					System.err.println(cause.getMessage());
				}
			}
		}
		// check the current state and acts accordlying
		List<String> grantedPermissions;
		String[] perms;
		switch (state) {
		case CLOSED:
			Log.i("QFacebook", "Facebook State is CLOSED");
			onFacebookStateChanged( 6, new String[0] );
		break;
		case CLOSED_LOGIN_FAILED:
			Log.i("QFacebook", "Facebook State is CLOSED_LOGIN_FAILED");
			onFacebookStateChanged( 5, new String[0] );
		break;
		case CREATED:
			Log.i("QFacebook", "Facebook State is CREATED");
			onFacebookStateChanged( 0, new String[0] );
		break;
		case CREATED_TOKEN_LOADED:
			Log.i("QFacebook", "Facebook State is CREATED_TOKEN_LOADED");
			onFacebookStateChanged( 1, new String[0] );
		break;
		case OPENING:
			Log.i("QFacebook", "Facebook State is OPENING");
			onFacebookStateChanged( 2, new String[0] );
		break;
		case OPENED:
			Log.i("QFacebook", "Facebook State is OPENED");
			grantedPermissions = session.getPermissions();
			perms = grantedPermissions.toArray(new String[grantedPermissions.size()]);
			onFacebookStateChanged( 3, perms );
		break;
		case OPENED_TOKEN_UPDATED:
			Log.i("QFacebook", "Facebook State is OPENED_TOKEN_UPDATED");
			grantedPermissions = session.getPermissions();
			perms = grantedPermissions.toArray(new String[grantedPermissions.size()]);
			onFacebookStateChanged( 4, perms );
		break;
		}
	}

	// Send back to the private slot on QFacebook class
	private static native void onFacebookStateChanged( int newstate, String[] grantedPermissions );
	// Emit signal for operation done
	private static native void operationDone( String operation );
}
