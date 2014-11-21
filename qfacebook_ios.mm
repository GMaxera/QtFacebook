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

/*! Objective-C class for responding to Facebook Notifications
 *  --- NOT USED ----
@interface FbNotificationObserver : NSObject
@property QFacebook* qfacebook;
- (void)activeSessionNotification:(NSNotification*)notification;
- (void)closedSessionNotification:(NSNotification*)notification;
@end
@implementation FbNotificationObserver
- (void)activeSessionNotification:(NSNotification*)notification {
	NSLog(@"Facebook Notification: %@", [notification name]);
}
- (void)closedSessionNotification:(NSNotification*)notification {
	NSLog(@"Facebook Notification: %@", [notification name]);
}
@end
*/

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
	[[FBSession activeSession] handleOpenURL:url];
}
@end

class QFacebookPlatformData {
public:
	QFacebookPlatformData() {
		/*
		observer = [[FbNotificationObserver alloc] init];
		[[NSNotificationCenter defaultCenter] addObserver:observer
				selector:@selector(activeSessionNotification:)
				name:FBSessionDidBecomeOpenActiveSessionNotification
				object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:observer
				selector:@selector(closedSessionNotification:)
				name:FBSessionDidBecomeClosedActiveSessionNotification
				object:nil];
		*/
	}
	~QFacebookPlatformData() {
		//[[NSNotificationCenter defaultCenter] removeObserver:observer];
	}
	//FbNotificationObserver* observer;
};

void QFacebook::initPlatformData() {
	appID = QString::fromNSString( [FBSettings defaultAppID] );
	displayName = QString::fromNSString( [FBSettings defaultDisplayName] );
	switch( [[FBSession activeSession] state] ) {
	case FBSessionStateCreated:
		state = SessionCreated;
		connected = false;
		break;
	case FBSessionStateCreatedTokenLoaded:
		state = SessionCreatedTokenLoaded;
		connected = false;
		break;
	case FBSessionStateCreatedOpening:
		state = SessionOpening;
		connected = false;
		break;
	case FBSessionStateOpen:
		state = SessionOpen;
		connected = true;
		break;
	case FBSessionStateOpenTokenExtended:
		state = SessionOpenTokenExtended;
		connected = true;
		break;
	case FBSessionStateClosedLoginFailed:
		state = SessionClosedLoginFailed;
		connected = false;
		break;
	case FBSessionStateClosed:
		state = SessionClosed;
		connected = false;
		break;
	}
	data = new QFacebookPlatformData();
}

void QFacebook::login( QStringList permissions ) {
	if ( [[FBSession activeSession] isOpen] ) {
		//! already connected - do nothing
	}
	NSMutableArray *nsPermissions = [NSMutableArray arrayWithCapacity:permissions.size()];
	for( int i=0; i<permissions.size(); ++i ) {
		[nsPermissions addObject:permissions[i].toNSString()];
	}
	FBSession* fbSession = [[FBSession alloc] initWithPermissions:nsPermissions];
	[fbSession setStateChangeHandler:^(FBSession*, FBSessionState fstate, NSError* error) {
		if (error) {
			NSLog(@"error:%@",error);
		}
		switch( fstate ) {
		case FBSessionStateCreated:
			state = SessionCreated;
			connected = false;
			break;
		case FBSessionStateCreatedTokenLoaded:
			state = SessionCreatedTokenLoaded;
			connected = false;
			break;
		case FBSessionStateCreatedOpening:
			state = SessionOpening;
			connected = false;
			break;
		case FBSessionStateOpen:
			state = SessionOpen;
			connected = true;
			break;
		case FBSessionStateOpenTokenExtended:
			state = SessionOpenTokenExtended;
			connected = true;
			break;
		case FBSessionStateClosedLoginFailed:
			state = SessionClosedLoginFailed;
			connected = false;
			break;
		case FBSessionStateClosed:
			state = SessionClosed;
			connected = false;
			break;
		}
		emit stateChanged( state );
		emit connectedChanged( connected );
	}];
	[FBSession setActiveSession:fbSession];
	// for forcing the in-app login using webview: FBSessionLoginBehaviorForcingWebView
	// default FBSessionLoginBehaviorWithFallbackToWebView
	[fbSession openWithBehavior:FBSessionLoginBehaviorWithFallbackToWebView completionHandler:nil];
}

void QFacebook::close() {
	[[FBSession activeSession] closeAndClearTokenInformation];
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

void QFacebook::onApplicationStateChanged(Qt::ApplicationState state) {
	if ( state == Qt::ApplicationActive ) {
		[[FBSession activeSession] handleDidBecomeActive];
	}
}
