
INCLUDEPATH += $$PWD

SOURCES += \
	$$PWD/qfacebook.cpp

HEADERS += \
	$$PWD/qfacebook.h

android {
	QT += androidextras
	SOURCES += $$PWD/qfacebook_android.cpp

	QA_JAVASRC.path = /src
	QA_JAVASRC.files += $$PWD/Android/src/org/gmaxera/qtfacebook/*.java

	INSTALLS += QA_JAVASRC

} else:ios {
	## the objective sources should be put in this variable
	OBJECTIVE_SOURCES += \
		$$PWD/qfacebook_ios.mm
} else {
	SOURCES += \
		$$PWD/qfacebook_desktop.cpp
}
