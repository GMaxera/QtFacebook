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
	QAndroidJniObject defAppId = QAndroidJniObject::callStaticObjectMethod<jstring>(
				"com.facebook.Settings",
				"getApplicationId" );
	appID = defAppId.toString();
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
	// call the java implementation
	QAndroidJniObject::callStaticMethod<void>( data->jClassName.toLatin1().data(), "login" );

	// Checking exceptions
	QAndroidJniEnvironment env;
	if (env->ExceptionCheck()) {
		// Printing exception message
		env->ExceptionDescribe();

		// Clearing exceptions
		env->ExceptionClear();
	}
}

void QFacebook::close() {
	// call the java implementation
	QAndroidJniObject::callStaticMethod<void>( data->jClassName.toLatin1().data(), "close" );

	// Checking exceptions
	QAndroidJniEnvironment env;
	if (env->ExceptionCheck()) {
		// Printing exception message
		env->ExceptionDescribe();

		// Clearing exceptions
		env->ExceptionClear();
	}
}

void QFacebook::requestPublishPermissions() {
	// call the java implementation
	QAndroidJniObject::callStaticMethod<void>( data->jClassName.toLatin1().data(), "requestPublishPermissions" );

	// Checking exceptions
	QAndroidJniEnvironment env;
	if (env->ExceptionCheck()) {
		// Printing exception message
		env->ExceptionDescribe();

		// Clearing exceptions
		env->ExceptionClear();
	}
}

void QFacebook::publishPhoto( QPixmap photo, QString message ) {
	qDebug() << "Publish Photo" << photo.size() << message;

	QByteArray imgData;
	QBuffer buffer(&imgData);
	buffer.open(QIODevice::WriteOnly);
	photo.save(&buffer, "PNG");
	// create the java byte array
	QAndroidJniEnvironment env;
	jbyteArray imgBytes = env->NewByteArray( imgData.size() );
	env->SetByteArrayRegion( imgBytes, 0, imgData.size(), (jbyte*)imgData.constData() );
	// call the java implementation
	QAndroidJniObject::callStaticMethod<void>(
		data->jClassName.toLatin1().data(), "publishPhoto", "([BLjava/lang/String;)V", imgBytes,
		QAndroidJniObject::fromString(message).object<jstring>() );

	// Checking exceptions
	if (env->ExceptionCheck()) {
		// Printing exception message
		env->ExceptionDescribe();

		// Clearing exceptions
		env->ExceptionClear();
	}
}

void QFacebook::publishLinkViaShareDialog( QString linkName, QString link, QString imageUrl ) {
	qDebug() << "Publish link" << link << linkName << imageUrl;

	// call the java implementation
	QAndroidJniObject::callStaticMethod<void>(
		data->jClassName.toLatin1().data(), "publishLinkViaShareDialog",
		"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
		QAndroidJniObject::fromString(linkName).object<jstring>(),
		QAndroidJniObject::fromString(link).object<jstring>(),
		QAndroidJniObject::fromString(imageUrl).object<jstring>() );

	// Checking exceptions
	QAndroidJniEnvironment env;
	if (env->ExceptionCheck()) {
		// Printing exception message
		env->ExceptionDescribe();

		// Clearing exceptions
		env->ExceptionClear();
	}
}

void QFacebook::setAppID( QString ) {

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
			QAndroidJniObject::callStaticMethod<void>(
				data->jClassName.toLatin1().data(), "readPermissionsAdd", "(Ljava/lang/String;)V",
				QAndroidJniObject::fromString(permission).object<jstring>());
		} else {
			QAndroidJniObject::callStaticMethod<void>(
				data->jClassName.toLatin1().data(), "writePermissionsAdd", "(Ljava/lang/String;)V",
				QAndroidJniObject::fromString(permission).object<jstring>());
		}

		// Checking exceptions
		QAndroidJniEnvironment env;
		if (env->ExceptionCheck()) {
			// Printing exception message
			env->ExceptionDescribe();

			// Clearing exceptions
			env->ExceptionClear();
		}
	}
	emit requestPermissionsChanged( this->requestPermissions );
}

void QFacebook::addRequestPermission( QString requestPermission ) {
	if ( !requestPermissions.contains(requestPermission) ) {
		// add the permission
		requestPermissions.append( requestPermission );
		if ( isReadPermission(requestPermission) ) {
			QAndroidJniObject::callStaticMethod<void>(
				data->jClassName.toLatin1().data(), "readPermissionsAdd", "(Ljava/lang/String;)V",
				QAndroidJniObject::fromString(requestPermission).object<jstring>());
		} else {
			QAndroidJniObject::callStaticMethod<void>(
				data->jClassName.toLatin1().data(), "writePermissionsAdd", "(Ljava/lang/String;)V",
				QAndroidJniObject::fromString(requestPermission).object<jstring>());
		}

		// Checking exceptions
		QAndroidJniEnvironment env;
		if (env->ExceptionCheck()) {
			// Printing exception message
			env->ExceptionDescribe();

			// Clearing exceptions
			env->ExceptionClear();
		} else {
			emit requestPermissionsChanged(requestPermissions);
		}
	}
}

void QFacebook::onApplicationStateChanged(Qt::ApplicationState state) {
	Q_UNUSED(state);
	// NOT USED
}

static void fromJavaOnFacebookStateChanged(JNIEnv *env, jobject thiz, jint newstate, jobjectArray grantedPermissions ) {
	Q_UNUSED(env)
	Q_UNUSED(thiz)
	int state = newstate;
	QStringList permissions;
	int count = env->GetArrayLength(grantedPermissions);
	for( int i=0; i<count; i++ ) {
		QAndroidJniObject perm( env->GetObjectArrayElement(grantedPermissions, i) );
		permissions.append( perm.toString() );
	}
	if ( QFacebookPlatformData::initialized ) {
		qDebug() << "Calling Java Native";
		QMetaObject::invokeMethod(QFacebook::instance(), "onFacebookStateChanged", Qt::QueuedConnection,
			Q_ARG(int, state), Q_ARG(QStringList, permissions));

		// Checking exceptions
		QAndroidJniEnvironment env;
		if (env->ExceptionCheck()) {
			// Printing exception message
			env->ExceptionDescribe();

			// Clearing exceptions
			env->ExceptionClear();
		}
	} else {
		qDebug() << "Delay calling of slot onFacebookStateChanged";
		QFacebookPlatformData::stateAtStart = state;
		QFacebookPlatformData::grantedPermissionAtStart = permissions;
	}
}

static void fromJavaOnOperationDone(JNIEnv* env, jobject thiz, jstring operation ) {
	Q_UNUSED(env)
	Q_UNUSED(thiz)
	if ( QFacebookPlatformData::initialized ) {
		QString operationQ = QAndroidJniObject(operation).toString();
		QMetaObject::invokeMethod(QFacebook::instance(), "operationDone", Qt::QueuedConnection,
			Q_ARG(QString, operationQ) );

		// Checking exceptions
		QAndroidJniEnvironment env;
		if (env->ExceptionCheck()) {
			// Printing exception message
			env->ExceptionDescribe();

			// Clearing exceptions
			env->ExceptionClear();
		}
	}
}

static JNINativeMethod methods[] {
	{"onFacebookStateChanged", "(I[Ljava/lang/String;)V", (void*)(fromJavaOnFacebookStateChanged)},
	{"operationDone", "(Ljava/lang/String;)V", (void*)(fromJavaOnOperationDone)}
};

#ifdef QFACEBOOK_NOT_DEFINE_JNI_ONLOAD
int qFacebook_registerJavaNativeMethods(JavaVM* vm, void*) {
#else
extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
#endif
	JNIEnv *env;
	if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4) != JNI_OK) {
		return JNI_FALSE;
	}
	jclass clazz = env->FindClass("org/gmaxera/qtfacebook/QFacebookBinding");
	if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) < 0) {
		return JNI_FALSE;
	}
	return JNI_VERSION_1_4;
}
