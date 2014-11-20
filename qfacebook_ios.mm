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
#include <QString>

class QFacebookPlatformData {
public:
};

void QFacebook::initPlatformData() {
	data = new QFacebookPlatformData();
	NSLog(@"Starting Facebook State: %d", [[FBSession activeSession] state]);
}

void QFacebook::login( QStringList permissions ) {

	permissions << "email" << "public_profile";

	NSMutableArray *nsPermissions = [NSMutableArray arrayWithCapacity:permissions.size()];
	for( int i=0; i<permissions.size(); ++i ) {
		[nsPermissions addObject:permissions[i].toNSString()];
	}
	//FBSession* fbSession = [[FBSession alloc] initWithPermissions:nsPermissions];

	FBSession* fbSession = [FBSession activeSession];
	if ( [fbSession isOpen] ) {
		NSLog(@"FACEBOOK SESSION OPEN - Login Do Nothing");
		return;
	} else {
		[fbSession initWithPermissions:nsPermissions];
	}

	/*
	[fbSession setStateChangeHandler:^(FBSession*, FBSessionState state, NSError* error) {
		switch( state ) {
		case FBSessionStateCreated:
			NSLog(@"FACEBOOK STATE NOW: CREATED");
			break;
		case FBSessionStateCreatedTokenLoaded:
			NSLog(@"FACEBOOK STATE NOW: CREATED TOKEN LOADED");
			break;
		case FBSessionStateCreatedOpening:
			NSLog(@"FACEBOOK STATE NOW: CREATED OPENING");
			break;
		case FBSessionStateOpen:
			NSLog(@"FACEBOOK STATE NOW: OPEN");
			break;
		case FBSessionStateOpenTokenExtended:
			NSLog(@"FACEBOOK STATE NOW: OPEN TOKEN EXTENDED");
			break;
		case FBSessionStateClosedLoginFailed:
			NSLog(@"FACEBOOK STATE NOW: LOGIN FAILED");
			break;
		case FBSessionStateClosed:
			NSLog(@"FACEBOOK STATE NOW: CLOSED");
			break;
		}
	}];
	*/
	//[FBSession setActiveSession:fbSession];
	// for forcing the in-app login using webview: FBSessionLoginBehaviorForcingWebView
	// default FBSessionLoginBehaviorWithFallbackToWebView
	[fbSession openWithBehavior:FBSessionLoginBehaviorWithFallbackToWebView
				completionHandler:^(FBSession*, FBSessionState state, NSError* error) {
		if (error) {
			NSLog(@"error:%@",error);
		}
		NSLog(@"FACEBOOK STATE NOW: %d", state);
		switch( state ) {
		case FBSessionStateCreated:
			NSLog(@"FACEBOOK STATE NOW: CREATED");
			break;
		case FBSessionStateCreatedTokenLoaded:
			NSLog(@"FACEBOOK STATE NOW: CREATED TOKEN LOADED");
			break;
		case FBSessionStateCreatedOpening:
			NSLog(@"FACEBOOK STATE NOW: CREATED OPENING");
			break;
		case FBSessionStateOpen:
			NSLog(@"FACEBOOK STATE NOW: OPEN");
			break;
		case FBSessionStateOpenTokenExtended:
			NSLog(@"FACEBOOK STATE NOW: OPEN TOKEN EXTENDED");
			break;
		case FBSessionStateClosedLoginFailed:
			NSLog(@"FACEBOOK STATE NOW: LOGIN FAILED");
			break;
		case FBSessionStateClosed:
			NSLog(@"FACEBOOK STATE NOW: CLOSED");
			break;
		}
	}];
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
