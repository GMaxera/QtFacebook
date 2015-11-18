
INCLUDEPATH += $$PWD

SOURCES += \
	$$PWD/qfacebook.cpp

HEADERS += \
	$$PWD/qfacebook.h

android {
	QT += androidextras
	SOURCES += $$PWD/qfacebook_android.cpp

	QFACEBOOK_JAVASRC.path = /src/org/gmaxera/qtfacebook
	QFACEBOOK_JAVASRC.files += $$files($$PWD/Android/src/org/gmaxera/qtfacebook/*)
	INSTALLS += QFACEBOOK_JAVASRC
} else:ios {
	## the objective sources should be put in this variable
	OBJECTIVE_SOURCES += \
		$$PWD/qfacebook_ios.mm
} else {
	SOURCES += \
		$$PWD/qfacebook_desktop.cpp
}

OTHER_FILES += \
        $$PWD/README.md \
        $$files($$PWD/Android/src/org/gmaxera/qtfacebook/*)

