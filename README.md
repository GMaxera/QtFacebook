QtFacebook
==========

Code for use Facebook SDK from C++ and Qt Quick 2 in Qt 5 projects targeted on mobile devices (Android &amp; iOS)

The aim of this project is to provide code to include into your Qt project in order to use Facebook functionality easily. Hence, it provides ready-to-use code for common scenarion of login and sharing with Facebook, but you need to manually install into your Qt project.
Binary packages and plugins to install into Qt distributions are not provided.

Include QtFacebook in your project including the qtfacebook.pri in your .pro file:
```
## include the QtFacebook library as project
include(/Adjust/the/path/to/repos/QtFacebook/qtfacebook.pri)
```

How to use for iOS platform
==========
## Prepare Facebook SDK for iOS

* Install the Facebook SDK package
* Add the following flags to Qt project:
```
## Facebook SDK framework
LIBS += -F/path/to/FacebookSDK -framework FacebookSDK
```
**Important Info:** This only works with iOs Facebook SDK version *3.x* 
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

These instructions are only for gradle.

The Facebook android sdk is available from Maven Central, so you can avoid downloading the sdk directly. Simply open build.gradle and add the following lines just before the dependencies section:
```
repositories {
	mavenCentral()
}
```
and the following line into the dependencies section (note that QFacebook has been tested with Facebook SDK 3.x only):
```
compile 'com.facebook.android:facebook-android-sdk:3+'
```

### Integrating QtFacebook into your code
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
* Create a custom Activity for your app extending from QtActivity (this is needed because there are some methods that interacts with Facebook login/share flows), and override the following methods:
```
import org.qtproject.qt5.android.bindings.QtActivity;
import org.gmaxera.qtfacebook.QFacebookBinding;
import android.content.Intent;
import android.os.Bundle;

public class MyCustomAppActivity extends QtActivity {
	@Override
	public void onCreate(Bundle bundle) {
		super.onCreate(bundle);
		QFacebookBinding.onCreate(this, bundle);
		QFacebookBinding.setApplicationName(getString(R.string.app_name));
	}
	@Override
	protected void onResume() {
		super.onResume();
		QFacebookBinding.onResume();
	}
	@Override
	protected void onSaveInstanceState(Bundle outState) {
		super.onSaveInstanceState(outState);
		QFacebookBinding.onSaveInstanceState(outState);
	}
	@Override
	public void onPause() {
		super.onPause();
		QFacebookBinding.onPause();
	}
	@Override
	public void onDestroy() {
		QFacebookBinding.onDestroy();
		super.onDestroy();
	}
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		QFacebookBinding.onActivityResult(requestCode, resultCode, data);
	}
}
```

## Warnings and Know issues

### ANDROID_PACKAGE_SOURCE_DIR Problem with QtFacebook
If in your project you setup the ANDROID_PACKAGE_SOURCE_DIR and if the directory specified contains a "src" subdirectory, then all java source files from QtFacebook will not copied. For avoid this problem use the following workaround:
* move your "src" directory outside ANDROID_PACKAGE_SOURCE_DIR
* install your java file using the following method:
```
ANDROID_JAVA_SRCS.path = /src/com/yourpackage/name/
ANDROID_JAVA_SRCS.files = $$files($$PWD/java/*.java)
INSTALLS += ANDROID_JAVA_SRCS
```

### Multiple definition of JNI_OnLoad
In order to register native methods for the Java binding, the source code define and implement the JNI_OnLoad for doing the registration of native methods. If the source code is included into a project that already defines the JNI_OnLoad for some other custom java binding, then this error arise.
To solve it, define into the .pro the following define:
```
DEFINES += QFACEBOOK_NOT_DEFINE_JNI_ONLOAD
```
And in your custom JNI_OnLoad call the following function:
```
#include "qfacebook.h"
jint JNICALL JNI_OnLoad(JavaVM *vm, void* ptr) {
	...
	qFacebook_registerJavaNativeMethods( vm, ptr );
	...
}
```

How to use for Desktop
==========

QtFacebook does not really provide integration for desktop applications. It does provide, however a dummy implementation of Facebook requests. This is useful because it permits to run your application on a desktop (e.g. to test other features) and to some very basic tests on facebook integration (e.g. to test if your code reacts correctly to a successful facebook login).

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
