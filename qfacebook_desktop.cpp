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

#include "qfacebook.h"
#include <QString>
#include <QDebug>
#include <QByteArray>
#include <QBuffer>

class QFacebookPlatformData {
public:
	QString jClassName;
	// this avoid to create the QFacebook from native method
	// when the Qt Application is not loaded yet
	static bool initialized;
	// init state and permission got from Facebook SDK before
	// the Qt Application loading
	static int stateAtStart;
	static QStringList grantedPermissionAtStart;
};

bool QFacebookPlatformData::initialized = false;
int QFacebookPlatformData::stateAtStart = -1;
QStringList QFacebookPlatformData::grantedPermissionAtStart = QStringList();

void QFacebook::initPlatformData() {
	displayName = "Not used on Android";
	data = new QFacebookPlatformData();
	data->jClassName = "org/gmaxera/qtfacebook/QFacebookBinding";
	// Get the default application ID
	appID = "dummy";
	data->initialized = true;
	qDebug() << "QFacebook Initialization:" << appID;
	if ( QFacebookPlatformData::stateAtStart != -1 ) {
		qDebug() << "Sync with state and permission loaded at start";
		onFacebookStateChanged( QFacebookPlatformData::stateAtStart,
								QFacebookPlatformData::grantedPermissionAtStart );
		qDebug() << state << grantedPermissions;
	}
}

void QFacebook::login() {
	// Directly calling slot
	onFacebookStateChanged(SessionOpen, QStringList());
}

void QFacebook::close() {
	// Directly calling slot
	onFacebookStateChanged(SessionClosed, QStringList());
}

void QFacebook::requestPublishPermissions() {
	// Directly calling slot
	onFacebookStateChanged(SessionOpenTokenExtended, QStringList());
}

void QFacebook::publishPhoto( QPixmap photo, QString message ) {
	Q_UNUSED(message)
	qDebug() << "Publish Photo" << photo.size() << message;
}

void QFacebook::publishLinkViaShareDialog( QString linkName, QString link, QString imageUrl ) {
	qDebug() << "Publish link" << link << linkName << imageUrl;
}

void QFacebook::setAppID( QString appID ) {
	Q_UNUSED(appID)

}

void QFacebook::setDisplayName( QString displayName ) {
	Q_UNUSED(displayName)
	// NOT USED
}

void QFacebook::setRequestPermissions( QStringList requestPermissions ) {
	this->requestPermissions = requestPermissions;
	emit requestPermissionsChanged( this->requestPermissions );
}

void QFacebook::addRequestPermission( QString requestPermission ) {
	if ( !requestPermissions.contains(requestPermission) ) {
		// add the permission
		requestPermissions.append( requestPermission );
		emit requestPermissionsChanged(requestPermissions);
	}
}

void QFacebook::onApplicationStateChanged(Qt::ApplicationState state) {
	Q_UNUSED(state);
	// NOT USED
}
