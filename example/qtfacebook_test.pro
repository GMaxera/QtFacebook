TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

#import qtfacebook
include(../source/qtfacebook.pri)

# Default rules for deployment.
include(deployment.pri)

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
	android/res/values/strings.xml \
	java/MyCustomAppActivity.java

android {
	ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
	ANDROID_JAVA_SOURCES.path = /src/org/qtproject/example
	ANDROID_JAVA_SOURCES.files = $$files($$PWD/java/*.java)
	INSTALLS += ANDROID_JAVA_SOURCES
}
