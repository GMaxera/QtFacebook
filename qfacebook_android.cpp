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
#include <QtAndroidExtras>
#include <QDebug>

class QFacebookPlatformData {
public:
	QString jClassName;
};

void QFacebook::initPlatformData() {
	displayName = "Not used on Android";
	data = new QFacebookPlatformData();
	data->jClassName = "org/gmaxera/qtfacebook/QFacebookBinding";
	// Get the default application ID
	QAndroidJniObject defAppId = QAndroidJniObject::callStaticObjectMethod<jstring>(
				"com.facebook.Settings",
				"getApplicationId" );
	appID = defAppId.toString();
	qDebug() << "QFacebook Initialization:" << appID;
}

void QFacebook::login( QStringList permissions ) {
	// call the java implementation
	QAndroidJniObject::callStaticMethod<void>( data->jClassName.toLatin1().data(), "login" );
}

void QFacebook::close() {
}

void QFacebook::setAppID( QString appID ) {

}

void QFacebook::setDisplayName( QString displayName ) {

}

void QFacebook::onApplicationStateChanged(Qt::ApplicationState state) {
}
