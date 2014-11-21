QtFacebook
==========

Code for use Facebook SDK from C++ and Qt Quick 2 in Qt 5 projects targeted on mobile devices (Android &amp; iOS)

The aim of this project is to provide code to include into your Qt project in order to use Facebook functionality easily. Hence, it provides ready-to-use code for common scenarion of login and sharing with Facebook, but you need to manually install into your Qt project.
Binary packages and plugins to install into Qt distributions are not provided.

How to use for iOS platform
==========
## Prepare Facebook SDK for iOS

* Install the Facebook SDK package
* Add the following flags to Qt project:
```
## Facebook SDK framework
LIBS += -F/path/to/FacebookSDK -framework FacebookSDK
INCLUDEPATH += /path/to/QtFacebook
HEADERS += \
	/path/to/QtFacebook/qfacebook.h
SOURCES += \
	/path/to/QtFacebook/qfacebook.cpp
OBJECTIVE_SOURCES += \
	/path/to/QtFacebook/qfacebook_ios.mm
```
* For a better integration with Facebook app, specify the application ID, display name and Url scheme into Info.plist:
```
<key>CFBundleDisplayName</key>
<string>Kotatsu Puzzle</string>
<key>FacebookAppID</key>
<string>726720204325518</string>
<key>FacebookDisplayName</key>
<string>Kotatsu Puzzle</string>
<key>CFBundleURLTypes</key>
<array>
		<dict>
				<key>CFBundleURLSchemes</key>
				<array>
						<string>fb726720204325518</string>
				</array>
		</dict>
</array>
```
If you don't specity the URL Scheme into Info.plist the login using the fast-app switching will not work, and the login will happens using a popup webview into the application (that in same case are better).

## Warnings and Know issues
In order to integrate the Facebook SDK into iOS application, some delegates needs to be implemented. One of those, regards the UIApplicationDelegate application:openURL:sourceApplication:annotation. In a fully native iOS application this is not a problem, but in Qt application there is already one delegate, QIOSApplicationDelegate, used by Qt and cannot be specified more than one delegate. At the moment there is a bug opened for solving this issues (QTBUG-38184), and in the meanwhile the only way is to exploit the functionality of Objective-C and do some sort of hacking to implement the delegate into QIOSApplicationDelegate.
This sort of hack use the Objective-C categories, and if in your project there is only QtFacebook that do this kind of hack to QIOSApplicationDelegate then all is fine. But, this way to solve the problem may conflict with some other libraries that apply the same hack to solve this issues. Pay attention.

How to use for Android platform
==========
## Prepare Facebook SDK for Android

* Unzip the facebook android sdk package
* Inside the unzipped directory, locate the subdirectory 'facebook' and copy it in another location (in a subdirectory of your Qt project it's fine)
* Open the copied directory and in a command line window execute the following command to create a custom build.xml ant build file (select the version of android sdk you are using):
```
android update project --path . --target android-19
```
* Add to the Qt project a custom Android package source directory
```
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/Android
```
* In the Android package source directory add the project.properties file (or edit it) and append the following content (pay attention to android.library.reference.1, if you already added other library references you should ensure that the number is unique and progressive):
```
# Project target.
target=android-19
## Reference to the Facebook SDK
android.library.reference.1=../../facebook
```
* For a better integration of Facebook SDK, edit the AndroidManifest.xml and add the following tags into the <application> tag:
```
<activity android:name="com.facebook.LoginActivity" android:label="@string/app_name" android:theme="@android:style/Theme.Translucent.NoTitleBar"></activity>
<meta-data android:name="com.facebook.sdk.ApplicationId" android:value="@string/app_id"/>
```
* Check that AndroidManifest.xml contains the permission for using Internet:
```
<uses-permission android:name="android.permission.INTERNET"/>
```
* Edit the 'res/values/strings.xml' file present into the Android package source and add the following lines with Facebook display name and application id:
```
<string name="app_name">Display Name of Facebook App</string>
<string name="app_id">2281942447348331</string>
```
* Copy the Java bindings sources of QtFacebook into the 'src' folder of Android package source mantaining the directory structure (needed by Java to resolve the java packages). So, you should get a path like: Android/src/org/gmaxera/qtfacebook/QFacebookBinding.java
* In your Qt project add the following option:
```
QT += androidextras
INCLUDEPATH += /path/to/QtFacebook
HEADERS += \
	/path/to/QtFacebook/qfacebook.h
SOURCES += \
	/path/to/QtFacebook/qfacebook.cpp \
	/path/to/QtFacebook/qfacebook_android.cpp
```


How to use in Qt Quick
==========

* in your main.cpp (or more appropriate point depending on the structure of your app) register the QFacebook object as Singleton (you cannot use more instances of QFacebook) with the following code:
```
#include "qfacebook.h"
...
qmlRegisterSingletonType<QFacebook>("com.yourcompany.yourapp", 1, 0, "Facebook", QFacebook::qFacebookProvider);
```
* in Qt Quick 2 source, use the QFacebook with the following code:
```
import com.yourcompany.yourapp 1.0
MouseArea {
	onClicked: Facebook.login()
}
```
