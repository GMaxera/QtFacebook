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
