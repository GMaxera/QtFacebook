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
#import "FacebookSDK/FacebookSDK.h"
#import "UIKit/UIKit.h"
#include <QString>
#include <QPixmap>
#include <QByteArray>
#include <QBuffer>

/*! Override the application:openURL UIApplicationDelegate adding
 *  a category to the QIOApplicationDelegate.
 *  The only way to do that even if it's a bit like hacking the Qt stuff
 *  See: https://bugreports.qt-project.org/browse/QTBUG-38184
 */
@interface QIOSApplicationDelegate
@end
//! Add a category to QIOSApplicationDelegate
@interface QIOSApplicationDelegate (QFacebookApplicationDelegate)
@end
//! Now add method for handling the openURL from Facebook Login
@implementation QIOSApplicationDelegate (QFacebookApplicationDelegate)
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString*) sourceApplication annotation:(id)annotation {
#pragma unused(application)
#pragma unused(sourceApplication)
#pragma unused(annotation)
	return [[FBSession activeSession] handleOpenURL:url];
}
@end

class QFacebookPlatformData {
public:
	QFacebook* qFacebook;
	void sessionStateHandler(FBSession* session, FBSessionState fstate, NSError* error) {
		if (error) {
			NSLog(@"error:%@",error);
		}
		QStringList grantedList;
		switch( fstate ) {
		case FBSessionStateCreated:
			qFacebook->onFacebookStateChanged( QFacebook::SessionCreated, QStringList() );
			break;
		case FBSessionStateCreatedTokenLoaded:
			qFacebook->onFacebookStateChanged( QFacebook::SessionCreatedTokenLoaded, QStringList() );
			break;
		case FBSessionStateCreatedOpening:
			qFacebook->onFacebookStateChanged( QFacebook::SessionCreatedTokenLoaded, QStringList() );
			break;
		case FBSessionStateOpen:
			for( NSString* perm in [session permissions] ) {
				grantedList.append( QString::fromNSString(perm) );
			}
			qFacebook->onFacebookStateChanged( QFacebook::SessionOpen, grantedList );
			break;
		case FBSessionStateOpenTokenExtended:
			for( NSString* perm in [session permissions] ) {
				grantedList.append( QString::fromNSString(perm) );
			}
			qFacebook->onFacebookStateChanged( QFacebook::SessionOpenTokenExtended, grantedList );
			break;
		case FBSessionStateClosedLoginFailed:
			qFacebook->onFacebookStateChanged( QFacebook::SessionClosedLoginFailed, QStringList() );
			break;
		case FBSessionStateClosed:
			qFacebook->onFacebookStateChanged( QFacebook::SessionClosed, QStringList() );
			break;
		}
	}
	//! subset of requestPermissions that only allow reading from Facebook
	NSMutableArray* readPermissions;
	//! subset of requestPermissions that allow writing to Facebook
	NSMutableArray* writePermissions;
};

void QFacebook::initPlatformData() {
	appID = QString::fromNSString( [FBSettings defaultAppID] );
	displayName = QString::fromNSString( [FBSettings defaultDisplayName] );
	data = new QFacebookPlatformData();
	data->qFacebook = this;
	data->readPermissions = [[NSMutableArray alloc] init];
	data->writePermissions = [[NSMutableArray alloc] init];
	[[FBSession activeSession]
			setStateChangeHandler:^(FBSession* session, FBSessionState state, NSError* error) {
				data->sessionStateHandler(session, state, error);
	}];
	data->sessionStateHandler( [FBSession activeSession], [[FBSession activeSession] state], NULL );
}

void QFacebook::login() {
	FBSession* fbSession = [[FBSession alloc] initWithPermissions:(data->readPermissions)];
	[fbSession setStateChangeHandler:^(FBSession* session, FBSessionState state, NSError* error) {
		data->sessionStateHandler(session, state, error);
	}];
	[FBSession setActiveSession:fbSession];
	// for forcing the in-app login using webview: FBSessionLoginBehaviorForcingWebView
	// default FBSessionLoginBehaviorWithFallbackToWebView
	[fbSession openWithBehavior:FBSessionLoginBehaviorWithFallbackToWebView completionHandler:nil];
}

bool QFacebook::autoLogin() {
	FBSession* fbSession = [[FBSession alloc] initWithPermissions:(data->readPermissions)];
	[fbSession setStateChangeHandler:^(FBSession* session, FBSessionState state, NSError* error) {
		data->sessionStateHandler(session, state, error);
	}];
	[FBSession setActiveSession:fbSession];
	return [FBSession openActiveSessionWithAllowLoginUI:NO];
}

void QFacebook::close() {
	[[FBSession activeSession] closeAndClearTokenInformation];
}

void QFacebook::requestMe() {
	[FBRequestConnection
		startForMeWithCompletionHandler:^(FBRequestConnection *connection, id<FBGraphUser> result, NSError *error) {
		if (error) {
			emit operationError( "requestMe", QString::fromNSString([error localizedDescription]) );
		} else {
			QVariantMap data;
			data["id"] = QString::fromNSString(result.id);
			data["first_name"] = QString::fromNSString(result[@"first_name"]);
			data["last_name"] = QString::fromNSString(result[@"last_name"]);
			data["email"] = QString::fromNSString(result[@"email"]);
			emit operationDone( "requestMe", data );
		}
	}];
}

void QFacebook::requestPublishPermissions() {
	[[FBSession activeSession] requestNewPublishPermissions:(data->writePermissions)
		defaultAudience:FBSessionDefaultAudienceFriends
		completionHandler:^( FBSession *session, NSError *error) {
		if (error) {
			emit operationError( "requestPublishPermissions", QString::fromNSString([error localizedDescription]) );
		}
	}];
}

void QFacebook::publishPhoto( QPixmap photo, QString message ) {
	//qDebug() << "Publish Photo" << photo.size() << message;

	QByteArray imgData;
	QBuffer buffer(&imgData);
	buffer.open(QIODevice::WriteOnly);
	photo.save(&buffer, "PNG");
	UIImage* img = [[UIImage imageWithData:(imgData.toNSData())] autorelease];

	NSMutableDictionary *params = [[NSMutableDictionary alloc] init];
	[params setObject:img forKey:@"source"];
	if ( !message.isEmpty() ) {
		[params setObject:message.toNSString() forKey:@"message"];
	}
	[FBRequestConnection
		startWithGraphPath:@"me/photos"
		parameters:params
		HTTPMethod:@"POST"
		completionHandler:^(FBRequestConnection* connection, id result, NSError *error) {
		if (error) {
			emit operationError( "publishPhoto", QString::fromNSString([error localizedDescription]) );
		} else {
			emit operationDone( "publishPhoto" );
		}
	}];
}

void QFacebook::publishPhotosViaShareDialog(QVariantList photos)
{
    qDebug() << "Publish Photos" << photos.size();
}

void QFacebook::publishLinkViaShareDialog( QString linkName, QString link, QString imageUrl, QString caption, QString description ) {
	qDebug() << "Publish link" << link << linkName << imageUrl << caption << description;
	// escaping the URL
	NSString* linkEscaped = [(link.toNSString()) stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
	NSString* imageEscaped = [(imageUrl.toNSString()) stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
	FBLinkShareParams* params = [[FBLinkShareParams alloc]
		initWithLink:[NSURL URLWithString:linkEscaped]
		name: (linkName.isEmpty() ? nil : linkName.toNSString())
		caption: (caption.isEmpty() ? nil : caption.toNSString())
		description: (description.isEmpty() ? nil : description.toNSString())
		picture: (imageUrl.isEmpty() ? nil : [NSURL URLWithString:imageEscaped])
	];
	
	if ( [FBDialogs canPresentShareDialogWithParams:params] ) {
		[FBDialogs presentShareDialogWithLink:params.link
			name:params.name
			caption:params.caption
			description:params.description
			picture:params.picture
			clientState:nil
			handler:^(FBAppCall *call, NSDictionary *results, NSError *error) {
				if (error) {
					emit operationError( "publishLinkViaShareDialog",
										 QString::fromNSString([error localizedDescription]) );
				} else {
					emit operationDone( "publishLinkViaShareDialog" );
				}
			}
		];
	} else {
		emit operationError( "publishLinkViaShareDialog", "Cannot present Facebook sharing dialog" );
	}
}

void QFacebook::requestMyFriends() {
	// Issue a Facebook Graph API request to get your user's friend list
	[FBRequestConnection startForMyFriendsWithCompletionHandler:^(FBRequestConnection *connection, id result, NSError *error) {
		if (!error) {
			// result will contain an array with your user's friends in the "data" key
			NSArray *friendObjects = [result objectForKey:@"data"];
			// Create a list of friends' Facebook IDs
			QStringList friendIds;
			for (NSDictionary *friendObject in friendObjects) {
				friendIds << QString::fromNSString([friendObject objectForKey:@"id"]);
			}
			QVariantMap dataMap;
			dataMap["friends"] = friendIds;
			emit operationDone( "requestMyFriends", dataMap );
		} else {
			emit operationError( "requestMyFriends",
								 QString::fromNSString([error localizedDescription]) );
		}
	}];
}

void QFacebook::setAppID( QString appID ) {
	if ( this->appID != appID ) {
		this->appID = appID;
		[FBSettings setDefaultAppID:(this->appID.toNSString())];
		emit appIDChanged( this->appID );
	}
}

void QFacebook::setDisplayName( QString displayName ) {
	if ( this->displayName != displayName ) {
		this->displayName = displayName;
		[FBSettings setDefaultDisplayName:(this->displayName.toNSString())];
		emit displayNameChanged( this->displayName );
	}
}

void QFacebook::setRequestPermissions( QStringList requestPermissions ) {
	this->requestPermissions = requestPermissions;
	[(data->readPermissions) removeAllObjects];
	[(data->writePermissions) removeAllObjects];
	foreach( QString permission, this->requestPermissions ) {
		if ( isReadPermission(permission) ) {
			[(data->readPermissions) addObject:permission.toNSString()];
		} else {
			[(data->writePermissions) addObject:permission.toNSString()];
		}
	}
	emit requestPermissionsChanged( this->requestPermissions );
}

void QFacebook::addRequestPermission( QString requestPermission ) {
	if ( !requestPermissions.contains(requestPermission) ) {
		// add the permission
		requestPermissions.append( requestPermission );
		if ( isReadPermission(requestPermission) ) {
			[(data->readPermissions) addObject:requestPermission.toNSString()];
		} else {
			[(data->writePermissions) addObject:requestPermission.toNSString()];
		}
		emit requestPermissionsChanged(requestPermissions);
	}
}

QString QFacebook::getAccessToken() {
	return QString::fromNSString([[[FBSession activeSession] accessTokenData] accessToken]);
}

QString QFacebook::getExpirationDate() {
	NSDateFormatter* dateFormatter = [[NSDateFormatter alloc] init];
	NSLocale* enUSPOSIXLocale = [NSLocale localeWithLocaleIdentifier:@"en_US_POSIX"];
	[dateFormatter setLocale:enUSPOSIXLocale];
	[dateFormatter setDateFormat:@"yyyy-MM-dd'T'HH:mm:ss.SSS'Z'"];

	NSDate* date = [[[FBSession activeSession] accessTokenData] expirationDate];
	return QString::fromNSString( [dateFormatter stringFromDate:date] );
}

void QFacebook::onApplicationStateChanged(Qt::ApplicationState state) {
	if ( state == Qt::ApplicationActive ) {
		[[FBSession activeSession] handleDidBecomeActive];
	}
}
