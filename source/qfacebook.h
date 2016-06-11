/* ************************************************************************
 * Copyright (c) 2014 GMaxera <gmaxera@gmail.com>                         *
 *                                                                        *
 * This file is part of QtFacebook                                        *
 *                                                                        *
 * QtFacebook is free software: you can redistribute it and/or modify     *
 * it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License, or      *
 * (at your option) any later version.                                    *
 *                                                                        *
 * This program is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                   *
 * See the GNU General Public License for more details.                   *
 *                                                                        *
 * You should have received a copy of the GNU General Public License      *
 * along with this program. If not, see <http://www.gnu.org/licenses/>.   *
 * ********************************************************************** */

#include <QObject>
#include <QStringList>
#include <QtGui/QGuiApplication>
#include <QtQml>
#include <QUrl>
#include <QPixmap>

class QFacebookPlatformData;
class QQuickItemGrabResult;

#ifdef QFACEBOOK_NOT_DEFINE_JNI_ONLOAD
typedef struct _JavaVM JavaVM;
/*! this function register native method to the Java binding class */
int qFacebook_registerJavaNativeMethods(JavaVM*, void*);
#endif

/*! QFacebook object allow to access a various functionality of Facebook SDK
 *  in a simpler way and on different platform with the same interface.
 *
 *  The supported platform for now are: Android and iOS
 */
class QFacebook : public QObject {
	Q_OBJECT
	Q_ENUMS( FacebookState )
	/*! Facebook application ID */
	Q_PROPERTY( QString appID READ getAppID WRITE setAppID NOTIFY appIDChanged )
	/*! Facebook application display name (used only on iOS platform) */
	Q_PROPERTY( QString displayName READ getDisplayName WRITE setDisplayName NOTIFY displayNameChanged )
	/*! True if the login into Facebook has been done and the session is active, False otherwise */
	Q_PROPERTY( bool connected READ getConnected NOTIFY connectedChanged )
	/*! Facebook Session Current State (detailed state as returned by Facebook SDK) */
	Q_PROPERTY( FacebookState state READ getState NOTIFY stateChanged )
	/*! The list of all requested permissions; use this to configure the permission needed
	 *  by your application
	 *  Any changes in the requestPermissions does not immediately correspond to a change into
	 *  the actual granted permissions on the active Facebook session. The permissions will be requested
	 *  and (if user allow) granted only during the Facebook operation that requires the permissions.
	 *  For example, during the login none of write permissions will be requested even if they are in
	 *  this list.
	 */
	Q_PROPERTY( QStringList requestPermissions READ getRequestPermissions WRITE setRequestPermissions NOTIFY requestPermissionsChanged )
	/*! The list of all granted permissions (may differs from what you request on requestPermissions) */
	Q_PROPERTY( QStringList grantedPermissions READ getGrantedPermissions NOTIFY grantedPermissionsChanged )
public:
	/*! singleton type provider function for Qt Quick */
	static QObject* qFacebookProvider(QQmlEngine *engine, QJSEngine *scriptEngine);
	/*! singleton object provider for C++ */
	static QFacebook* instance();
	/*! enum of possible state for Facebook Session */
	enum FacebookState {
		/*! One of two initial states indicating that no valid cached token was found */
		SessionCreated = 0,
		/*! One of two initial session states indicating that a cached token was loaded;
		 *  when a session is in this state, a call to login will result in an open session,
		 *  without UX or app-switching */
		SessionCreatedTokenLoaded = 1,
		/*! One of three pre-open session states indicating that an attempt to open
		 *  the session is underway */
		SessionOpening = 2,
		/*! Open session state indicating user has logged in or a cached token is available */
		SessionOpen = 3,
		/*! Open session state indicating token has been extended, or the user has
		 *  granted additional permissions */
		SessionOpenTokenExtended = 4,
		/*! Closed session state indicating that a login attempt failed */
		SessionClosedLoginFailed = 5,
		/*! Closed session state indicating that the session was closed,
		 *  but the users token remains cached on the device for later use */
		SessionClosed = 6
	};
public slots:
	/*! perform a login into facebook
	 *  During the login to Facebook only the read permissions (which that doesn't allow
	 *  to publish) listed into requestPermissions property are requested to the user.
	 *  To obtain the permissions for publish, you have to call the requestPublishPermissions after
	 *  the login.
	 */
	void login();
	/*! try to login into facebook automatically take data from cache
	 *  This call is synchronous !
	 *  \return true if the login was successfull
	 */
	bool autoLogin();
	/*! close the Facebook session and clear any cached information */
	void close();
	/*! request information about the connected user (me)
	 *  The data is returned into as a QVariantMap into the QVariant
	 *  sent via operationDone signal
	 */
	void requestMe();
	/*! request write permissions for publish on Facebook
	 *  Call this method only after a successfull login to Facebook
	 */
	void requestPublishPermissions();
	/*! post a photo to the user wall
	 *  \param photo the image will be uploaded to the user album on Facebook
	 *  \param message an optional description of the photo that will be shown in the feed story
	 */
	void publishPhoto( QPixmap photo, QString message=QString() );
    /*! post a photo to the user wall
     *  \param result the QQuickItem::grabToImage() result will be uploaded to the user album on Facebook
     *  \param message an optional description of the photo that will be shown in the feed story
     */
    void publishQuickItemGrabResult( QQuickItemGrabResult *result, QString message=QString() );

    /*! Publish a photo using Photo Share Dialog.
     *
     *  If the Photo Share Dialog is not available
     *  (e.g. because the user hasn't the Facebook app installed), then do nothing
     *  (TODO: Fix it, try to using Feed Dialog).
     *  This function does not require the user to be logged into Facebook from the app.
     *  \param photos photos to publish, supported types: QPixmap and QQuickItemGrabResult
     */
    void publishPhotosViaShareDialog( QVariantList photos );

	/*! Publish a link with a photo using Share Dialog.
	 *
	 *  If the Share Dialog is not available (e.g. because the user hasn't the Facebook app installed),
	 *  falls back to using the Feed Dialog. This function does not require the user to be logged into
	 *  Facebook from the app
	 *  \param linkName the name of the link
	 *  \param link the link url
	 *  \param imageUrl is the url of the image associated wih the link
	 *  \param caption the text to be used as caption
	 *  \param description the test to be used as description
	 */
	void publishLinkViaShareDialog( QString linkName, QString link, QString imageUrl, QString caption, QString description );
	/*! Request the list of facebook friends using the app
	 *  The list is returned via the signal operationDone into a QVariantMap at
	 *  the key "friends" there will be a QStringList containing the facebookId of friends
	 */
	void requestMyFriends();

	/*! return the application ID */
	QString getAppID();
	/*! configure the application ID (it is a global settings for all future sessions) */
	void setAppID( QString appID );
	/*! return the display name of the Facebook application */
	QString getDisplayName();
	/*! configure the display name of Facebook application
	 *  (it must match the name configured on the developer Facebook portal) */
	void setDisplayName( QString displayName );
	/*! True if connected to Facebook and session is active; false otherwise */
	bool getConnected();
	/*! return the current state of Facebook session */
	FacebookState getState();
	/*! return the list of requested permissions */
	QStringList getRequestPermissions();
	/*! change the whole list of request permissions */
	void setRequestPermissions( QStringList requestPermissions );
	/*! add a new permission to request permission */
	void addRequestPermission( QString requestPermission );
	/*! return the current granted permissions from the active Facebook session */
	QStringList getGrantedPermissions();
	/*! return the access token (only valid when a token has been loaded) */
	QString getAccessToken();
	/*! return the expire date of the access token (only valid when a token has been loaded)
	 *  The data is returned as a string formatted into ISO format yyyy-MM-ddTHH:mm:ss.SSSZ
	 */
	QString getExpirationDate();
signals:
	void appIDChanged( QString appID );
	void displayNameChanged( QString displayName );
	void connectedChanged( bool connected );
	void stateChanged( FacebookState state );
	void requestPermissionsChanged( QStringList requestPermissions );
	void grantedPermissionsChanged( QStringList grantedPermissions );
	/*! emitted when an operation is completed
	 *  \param operation the name of the method called (i.e. publishPhoto)
	 *  \param data eventually data returned by the operation completed
	 *  \note login and close does not emit this signal; use connected and state properties
	 *  to monitor login and close operations
	 */
	void operationDone( QString operation, QVariantMap data=QVariantMap() );
	/*! emitted when an error occur during a Facebook operation
	 *  \param operation the name of the method called (i.e. publishPhoto)
	 *  \param error the error returned by Facebook
	 *  \note A failed login is not an error
	 */
	void operationError( QString operation, QString error );
private slots:
	//! handle the return to the active state for supporting app-switch login
	void onApplicationStateChanged(Qt::ApplicationState state);
	//! handle the changing of the underlying Facebook session state
	void onFacebookStateChanged( int newstate, QStringList grantedPermissions );
private:
	/*! singleton object */
	QFacebook( QObject* parent=0 );
	~QFacebook();
	Q_DISABLE_COPY(QFacebook)

	/*! check if a requested permission is read-only or write
	 *  If it doesn't recognize the permission, it will be considered write permission
	 */
	bool isReadPermission( QString permission );

	QString appID;
	QString displayName;
	bool connected;
	FacebookState state;
	QStringList requestPermissions;
	QStringList grantedPermissions;
	/*! Platform specific data */
	QFacebookPlatformData* data;
	/*! initialized the platform specific data */
	void initPlatformData();
	friend class QFacebookPlatformData;
};
