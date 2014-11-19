QtFacebook
==========

Code for use Facebook SDK from C++ and Qt Quick 2 in Qt 5 projects targeted on mobile devices (Android &amp; iOS)

How to use for iOS platform
==========
## Prepare Facebook SDK for iOS

* Install the Facebook SDK package
* Add the following flags to Qt project:
```
## Facebook SDK framework
LIBS += -F/path/to/FacebookSDK -framework FacebookSDK
```

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


