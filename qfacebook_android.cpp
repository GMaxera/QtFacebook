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
	void registerNativeMethods();
};

void QFacebook::initPlatformData() {
	displayName = "Not used on Android";
	data = new QFacebookPlatformData();
	data->jClassName = "org/gmaxera/qtfacebook/QFacebookBinding";
	data->registerNativeMethods();
	// Get the default application ID
	QAndroidJniObject defAppId = QAndroidJniObject::callStaticObjectMethod<jstring>(
				"com.facebook.Settings",
				"getApplicationId" );
	appID = defAppId.toString();
	qDebug() << "QFacebook Initialization:" << appID;
}

void QFacebook::login() {
	// call the java implementation
	QAndroidJniObject::callStaticMethod<void>( data->jClassName.toLatin1().data(), "login" );
}

void QFacebook::close() {
}

void QFacebook::setAppID( QString appID ) {

}

void QFacebook::setDisplayName( QString displayName ) {
	Q_UNUSED(displayName)
	// NOT USED
}

void QFacebook::setRequestPermissions( QStringList requestPermissions ) {
	this->requestPermissions = requestPermissions;
	QAndroidJniObject::callStaticMethod<void>( data->jClassName.toLatin1().data(), "readPermissionsClear" );
	QAndroidJniObject::callStaticMethod<void>( data->jClassName.toLatin1().data(), "writePermissionsClear" );
	foreach( QString permission, this->requestPermissions ) {
		if ( isReadPermission(permission) ) {
			QAndroidJniObject::callStaticMethod<void>( data->jClassName.toLatin1().data(),
													   "readPermissionsAdd",
													   "(Ljava/lang/String;)V",
								QAndroidJniObject::fromString(permission).object<jstring>());
		} else {
			QAndroidJniObject::callStaticMethod<void>( data->jClassName.toLatin1().data(),
													   "writePermissionsAdd",
													   "(Ljava/lang/String;)V",
							 QAndroidJniObject::fromString(permission).object<jstring>());
		}
	}
	emit requestPermissionsChanged( this->requestPermissions );
}

void QFacebook::addRequestPermission( QString requestPermission ) {
	if ( !requestPermissions.contains(requestPermission) ) {
		// add the permission
		requestPermissions.append( requestPermission );
		if ( isReadPermission(requestPermission) ) {
			QAndroidJniObject::callStaticMethod<void>( data->jClassName.toLatin1().data(),
													   "readPermissionsAdd",
													   "(Ljava/lang/String;)V",
								QAndroidJniObject::fromString(requestPermission).object<jstring>());
		} else {
			QAndroidJniObject::callStaticMethod<void>( data->jClassName.toLatin1().data(),
													   "writePermissionsAdd",
													   "(Ljava/lang/String;)V",
								QAndroidJniObject::fromString(requestPermission).object<jstring>());
		}
		emit requestPermissionsChanged(requestPermissions);
	}
}

void QFacebook::onApplicationStateChanged(Qt::ApplicationState state) {
	Q_UNUSED(state);
	// NOT USED
}

static void fromJavaOnFacebookStateChanged(JNIEnv *env, jobject thiz, jint newstate) {
	Q_UNUSED(env)
	Q_UNUSED(thiz)
	int state = newstate;
	QMetaObject::invokeMethod(QFacebook::instance(), "onFacebookStateChanged",
							  Qt::QueuedConnection,
							  Q_ARG(int, state));
}

void QFacebookPlatformData::registerNativeMethods() {
	JNINativeMethod methods[] {
		{"onFacebookStateChanged", "(I)V", reinterpret_cast<void *>(fromJavaOnFacebookStateChanged)}
	};

	QAndroidJniObject javaClass(jClassName.toLatin1().data());
	QAndroidJniEnvironment env;
	jclass objectClass = env->GetObjectClass(javaClass.object<jobject>());
	env->RegisterNatives(objectClass,
						 methods,
						 sizeof(methods) / sizeof(methods[0]));
	env->DeleteLocalRef(objectClass);
}
