QT      += core gui widgets network networkauth

TARGET   = EpisodiacV2
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        episodiac.cpp \
        thetvdb.cpp \
        rename.cpp \
        themoviedb.cpp \
        kodi.cpp \
        settings.cpp

HEADERS += \
        episodiac.h \
        thetvdb.h \
        rename.h \
        themoviedb.h \
        kodi.h \
        settings.h \
        data.h

FORMS   += \
        episodiac.ui \
        settings.ui

VERSION = 2.0.0.11
win32 {
    RC_ICONS = icon.ico
    QMAKE_TARGET_COMPANY = "GrafSauger"
    QMAKE_TARGET_PRODUCT = "Episodiac V2 - TV Show Rename Tool"
    QMAKE_TARGET_DESCRIPTION = "Episodiac V2 - TV Show Rename Tool"
    QMAKE_TARGET_COPYRIGHT = "2016 - 2023 by GrafSauger"

    DEFINES += APP_VERSION=\\\"$$VERSION\\\"
    DEFINES += QMAKE_TARGET_COMPANY=\\\"$$QMAKE_TARGET_COMPANY\\\"
    DEFINES += APP_PRODUCT=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\"
    DEFINES += QMAKE_TARGET_DESCRIPTION=\"\\\"$$QMAKE_TARGET_DESCRIPTION\\\"\"
    DEFINES += QMAKE_TARGET_COPYRIGHT=\"\\\"$$QMAKE_TARGET_COPYRIGHT\\\"\"
}

macx {
    ICON = episodiac.icns
}

DISTFILES +=

RESOURCES += \
    images.qrc
