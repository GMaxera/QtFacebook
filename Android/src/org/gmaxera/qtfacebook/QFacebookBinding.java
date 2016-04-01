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
import java.text.SimpleDateFormat;
import java.util.Locale;
import java.util.Collection;

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
	// A string set to the name of the Facebook dialog which we launched and
	// for which we are expecting notification via the uiLifecycleHelper.
	// Perhaps we should synchronize access to this variable? We could read
	// and modify it from different threads (though it is not so likely)
	private String runningFacebookDialog;
	// If this variable is not NULL, after a successful login is notified in
	// call(), the operation is executed in the gui thread
	private Runnable postLoginOperation = null;
	// The name of the application
	private String applicationName;
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
	//! Sets the application name
	static public void setApplicationName( String appName ) {
		m_instance.applicationName = appName;
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
				operationError(m_instance.runningFacebookDialog, error.toString());
			}

			@Override
			public void onComplete(FacebookDialog.PendingCall pendingCall, Bundle data) {
				Log.i("Activity", "Success!");
				operationDone(m_instance.runningFacebookDialog, new String[0]);
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
		createSessionIfNeeded();
		Session session = Session.getActiveSession();
		if ( !session.isOpened() ) {
			Session.OpenRequest request = new Session.OpenRequest(m_instance.activity);
			request.setPermissions( m_instance.readPermissions );
			session.openForRead( request );
			Log.i("QFacebook", "Requesting Login to Facebook");
		} else {
			Session.NewPermissionsRequest readRequest =
				new Session.NewPermissionsRequest(m_instance.activity, m_instance.readPermissions);
			session.requestNewReadPermissions( readRequest );
			Log.i("QFacebook", "Already connected to Facebook");
		}
	}

	// Perform an autologin into Facebook if possible
	static public boolean autoLogin() {
		createSessionIfNeeded();
		return (Session.openActiveSession(m_instance.activity, false, m_instance) != null);
	}

	// Perform the logout and clear any token information
	static public void close() {
		createSessionIfNeeded();
		Session.getActiveSession().closeAndClearTokenInformation();
	}

	// Request information about connected user (me)
	static public void requestMe() {
		Log.i("QFacebook", "Facebook start Request Me");
		createSessionIfNeeded();
		final Request request = Request.newMeRequest(Session.getActiveSession(),
			new Request.GraphUserCallback() {
			@Override
			public void onCompleted(GraphUser user, Response response) {
				FacebookRequestError error = response.getError();
				// Some errors occurs
				if ( error != null || user == null ) {
					Log.i("QFacebook", "Response terminated with an Error");
					operationError( "requestMe", error.getErrorMessage() );
				} else {
					// construct the array of string with key,value sequence
					String email = "";
					if ( user.getProperty("email") != null ) {
						// user got a valid email
						email = user.getProperty("email").toString();
					}
					String[] data = {
						"id", user.getId(),
						"first_name", user.getFirstName(),
						"last_name", user.getLastName(),
						"email", email
					};
					operationDone( "requestMe", data );
				}
			}
		});
		//Request.executeBatchAsync(request);
		Thread thread = new Thread() {
			@Override
			public void run() {
				Request.executeAndWait(request);
			}
		};
		thread.start();
	}

	// Request information about my friends
	static public void requestMyFriends() {
		Log.i("QFacebook", "Facebook start Request My Friends");
		createSessionIfNeeded();
		final Request request = Request.newMyFriendsRequest(Session.getActiveSession(),
			new Request.GraphUserListCallback() {
			@Override
			public void onCompleted(List<GraphUser> friends, Response response) {
				FacebookRequestError error = response.getError();
				// Some errors occurs
				if ( error != null ) {
					Log.i("QFacebook", "Response terminated with an Error");
					operationError( "requestMyFriends", error.getErrorMessage() );
				} else {
					// construct the array of string with key,value sequence
					String friendsList = "";
					if (friends != null) {
						for (GraphUser user : friends) {
							friendsList = friendsList.concat( user.getId() ).concat(",");
						}
					}
					String[] data = { "friends:list", friendsList };
					operationDone( "requestMyFriends", data );
				}
			}
		});
		//Request.executeBatchAsync(request);
		Thread thread = new Thread() {
			@Override
			public void run() {
				Request.executeAndWait(request);
			}
		};
		thread.start();
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
					GraphObject graphObject = response.getGraphObject();
					FacebookRequestError error = response.getError();
					if ( graphObject != null ) {
						Log.i("QFacebook", "Graph Object exists: "+graphObject.getProperty(Response.SUCCESS_KEY));
					}
					if ( error != null ) {
						Log.i("QFacebook", "Response terminated with an Error");
						operationError( "publishPhoto", error.getErrorMessage() );
					} else {
						Log.i("QFacebook", "Publish Photo Request Completed");
						operationDone( "publishPhoto", new String[0] );
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

        // Publish a photo using Photo Share Dialog. If the Photo Share Dialog is not available
        // (e.g. because the user hasn't the Facebook app installed), then do nothing
        // (TODO: Fix it, try to using Feed Dialog).
        // This function does not require the user to be logged into Facebook from the app.
        // imgBytes is photos in byte array presentation.
        static public void publishPhotosViaShareDialog( final byte[][] imgBytes ) {
                // Creating the session if it doesn't exist yet
                createSessionIfNeeded();

                // First of all checking if we can use the PhotoShareDialog and using it if we can
                if (FacebookDialog.canPresentShareDialog(m_instance.activity.getApplicationContext(),
						FacebookDialog.ShareDialogFeature.SHARE_DIALOG,
						FacebookDialog.ShareDialogFeature.PHOTOS)) {
                        Log.i("QFacebook", "Publishing using Photo Share Dialog");

                        m_instance.runningFacebookDialog = "publishPhotosViaShareDialog";

                        // Publish the post using the Photo Share Dialog. We start the dialog from the UI thread
                        m_instance.activity.runOnUiThread(new Runnable() {
                                public void run() {
									Collection<Bitmap> photos = new ArrayList<Bitmap>();
									for( byte[] img : imgBytes ) {
										Bitmap bitmap = BitmapFactory.decodeByteArray(img, 0, img.length);
										photos.add(bitmap);
									}

									FacebookDialog photoShareDialog = new FacebookDialog.PhotoShareDialogBuilder(m_instance.activity)
											.setApplicationName(m_instance.applicationName)
											.addPhotos(photos)
											.build();
									m_instance.uiLifecycleHelper.trackPendingDialogCall(photoShareDialog.present());
                                }
                        });
                } else {
                        Log.w("QFacebook", "Can not publish photo because facebook app is not installed");
                }
        }

	// Publish a link with a photo using Share Dialog. If the Share Dialog is not available
	// (e.g. because the user hasn't the Facebook app installed), falls back to using the
	// Feed Dialog. This function does not require the user to be logged into Facebook from
	// the app. linkName is the name of the link, link is the link url, imageUrl is the url
	// of the image associated wih the link.
	static public void publishLinkViaShareDialog( final String linkName, final String link, final String imageUrl, final String caption, final String description ) {
		// Creating the session if it doesn't exist yet
		createSessionIfNeeded();
		// First of all checking if we can use the ShareDialog and using it if we can
		if (FacebookDialog.canPresentShareDialog(m_instance.activity.getApplicationContext(), FacebookDialog.ShareDialogFeature.SHARE_DIALOG)) {
			Log.i("QFacebook", "Publishing using Share Dialog");

			m_instance.runningFacebookDialog = "publishLinkViaShareDialog";

			// Publish the post using the Share Dialog. We start the dialog from the UI thread
			m_instance.activity.runOnUiThread(new Runnable() {
				public void run() {
					FacebookDialog shareDialog = new FacebookDialog.ShareDialogBuilder(m_instance.activity)
						.setApplicationName(m_instance.applicationName)
						.setLink(link)
						.setName(linkName)
						.setPicture(imageUrl)
						.setCaption(caption)
						.setDescription(description)
						.build();
					m_instance.uiLifecycleHelper.trackPendingDialogCall(shareDialog.present());
				}
			});
		} else {
			Log.i("QFacebook", "Publishing using Feed Dialog");

			// Falling back to using the Feed Dialog. We need to perform the login, first

			// Creating the runnable to start after a successful login
			final Bundle params = new Bundle();
			params.putString("name", linkName);
			params.putString("link", link);
			params.putString("picture", imageUrl);
			params.putString("caption", caption);
			params.putString("description", description);

			m_instance.postLoginOperation = new Runnable() {
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
									String[] data = new String[2];
									// We fill array with a key followed by a value, as
									// that is how C++ expects stuffs
									data[0] = "postId";
									data[1] = postId;
									operationDone("publishLinkViaShareDialog", data);
								} else {
									// User clicked the Cancel button
									Log.i("QFacebook", "Publication cancelled");
									operationError("publishLinkViaShareDialog", "Publication cancelled");
								}
							} else if (error instanceof FacebookOperationCanceledException) {
								// User clicked the "x" button
								Log.i("QFacebook", "Publish cancelled");
								operationError("publishLinkViaShareDialog", "Publication cancelled");
							} else {
								// Generic, ex: network error
								Log.e("QFacebook", "Error posting story");
								operationError("publishLinkViaShareDialog", "Error posting story");
							}
						}
					});

					WebDialog feedDialog = feedDialogBuilder.build();
					feedDialog.show();
				}
			};

			// Logging in. If login is successful, the postLoginOperation will be executed
			login();
		}
	}

	// Return the access token
	static public String getAccessToken() {
		// Creating the session if it doesn't exist yet
		createSessionIfNeeded();
		return Session.getActiveSession().getAccessToken();
	}

	// Return the expiration date
	static public String getExpirationDate() {
		// Creating the session if it doesn't exist yet
		createSessionIfNeeded();
		SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS'Z'", Locale.UK);
		return sdf.format( Session.getActiveSession().getExpirationDate() );
	}

	// The Session.StatusCallback method
	@Override
	public void call(Session session, SessionState state, Exception exception) {
		Log.i("QFacebook", "Inside the StatusCallback method");

		// check if there was an exception
		if (exception != null) {
			if (exception instanceof FacebookOperationCanceledException &&
				!SessionState.OPENED_TOKEN_UPDATED.equals(state)) {
				// HERE THE USER DID NOT ACCEPT SOMETHING
				Log.i("QFacebook", "The user did not accept something...");
			} else {
				// qui forse fare logout o segnare come logged out
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
		boolean runPostLoginOperation = false;
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
			runPostLoginOperation = true;
		break;
		case OPENED_TOKEN_UPDATED:
			Log.i("QFacebook", "Facebook State is OPENED_TOKEN_UPDATED");
			grantedPermissions = session.getPermissions();
			perms = grantedPermissions.toArray(new String[grantedPermissions.size()]);
			onFacebookStateChanged( 4, perms );
			runPostLoginOperation = true;
		break;
		}

		if (runPostLoginOperation && (m_instance.postLoginOperation != null)) {
			m_instance.activity.runOnUiThread(m_instance.postLoginOperation);
			m_instance.postLoginOperation = null;
		}
	}

	// Send back to the private slot on QFacebook class
	private static native void onFacebookStateChanged( int newstate, String[] grantedPermissions );
	// Emit signal for operation done
	private static native void operationDone( String operation, String[] data );
	// Emit signal for operation error
	private static native void operationError( String operation, String error );
}
