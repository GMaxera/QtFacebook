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

class QFacebookPlatformData;

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
	/*! Facebook application display name */
	Q_PROPERTY( QString displayName READ getDisplayName WRITE setDisplayName NOTIFY displayNameChanged )
	/*! True if the login into Facebook has been done and the session is active, False otherwise */
	Q_PROPERTY( bool connected READ getConnected NOTIFY connectedChanged )
	/*! Facebook Session Current State (detailed state as returned by Facebook SDK) */
	Q_PROPERTY( FacebookState state READ getState NOTIFY stateChanged )
public:
	/*! singleton type provider function for Qt Quick */
	static QObject* qFacebookProvider(QQmlEngine *engine, QJSEngine *scriptEngine);
	/*! singleton object provider for C++ */
	static QFacebook* instance();
	/*! enum of possible state for Facebook Session */
	enum FacebookState {
		/*! One of two initial states indicating that no valid cached token was found */
		SessionCreated,
		/*! One of two initial session states indicating that a cached token was loaded;
		 *  when a session is in this state, a call to login will result in an open session,
		 *  without UX or app-switching */
		SessionCreatedTokenLoaded,
		/*! One of three pre-open session states indicating that an attempt to open
		 *  the session is underway */
		SessionOpening,
		/*! Open session state indicating user has logged in or a cached token is available */
		SessionOpen,
		/*! Open session state indicating token has been extended, or the user has
		 *  granted additional permissions */
		SessionOpenTokenExtended,
		/*! Closed session state indicating that a login attempt failed */
		SessionClosedLoginFailed,
		/*! Closed session state indicating that the session was closed,
		 *  but the users token remains cached on the device for later use */
		SessionClosed
	};
public slots:
	/*! perform a login into facebook
	 *  \param permissions are an optional list of facebook permissions to ask during
	 *         the login. The default permissions 'email', 'public_profile' and 'user_friends'
	 *         are automatically implied even if not specified
	 */
	void login( QStringList permissions=QStringList() );
	/*! close the Facebook session and clear any cached information */
	void close();
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
signals:
	void appIDChanged( QString appID );
	void displayNameChanged( QString displayName );
	void connectedChanged( bool connected );
	void stateChanged( FacebookState state );
private slots:
	//! handle the return to the active state for supporting app-switch login
	void onApplicationStateChanged(Qt::ApplicationState state);
private:
	/*! singleton object */
	QFacebook( QObject* parent=0 );
	Q_DISABLE_COPY(QFacebook)

	QString appID;
	QString displayName;
	bool connected;
	FacebookState state;
	/*! Platform specific data */
	QFacebookPlatformData* data;
	/*! initialized the platform specific data */
	void initPlatformData();
	friend class QFacebookPlatformData;
};
