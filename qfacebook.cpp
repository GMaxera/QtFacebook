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

QObject* QFacebook::qFacebookProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
	Q_UNUSED(engine)
	Q_UNUSED(scriptEngine)
	return QFacebook::instance();
}

QFacebook* QFacebook::instance() {
	static QFacebook* facebook = new QFacebook();
	return facebook;
}

QFacebook::QFacebook(QObject *parent )
	: QObject(parent)
	, appID()
	, displayName() {
	initPlatformData();
}

QString QFacebook::getAppID() {
	return appID;
}

QString QFacebook::getDisplayName() {
	return displayName;
}

QFacebook::FacebookState QFacebook::getState() {
	return state;
}

