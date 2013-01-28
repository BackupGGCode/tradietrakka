# -------------------------------------------------
# Project created by QtCreator 2010-06-01T14:53:02
# -------------------------------------------------
TARGET = tradietrakka
TEMPLATE = app

include(Dialogs/Dialogs.pri)
include(Widgets/Widgets.pri)
include(BusinessLogic/BusinessLogic.pri)

SOURCES += main.cpp \
    mainwindow.cpp \
    firstrunwiz.cpp \
    appsettings.cpp \
    navpanel.cpp \
    imgobject.cpp \
    animations.cpp \
    csqltablemodel.cpp
    
HEADERS += mainwindow.h \
    firstrunwiz.h \
    appsettings.h \
    navpanel.h \
    imgobject.h \
    csqltablemodel.h
    
FORMS += mainwindow.ui

RESOURCES += Resources/TradieTrakka.qrc
QT += sql

OTHER_FILES += Resources/tradietrakka.desktop

win32{
	CONFIG -= console
	RC_FILE = Resources/icon.rc
}

macx{
    ICON = Resources/icons/tt.icns
}

unix{
    isEmpty(PREFIX) {
        PREFIX = /usr
    }
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share

    INSTALLS += target desktop icon

    target.path += $$BINDIR

    desktop.path = $$DATADIR/applications
    desktop.files += Resources/tradietrakka.desktop

    icon.path = $$DATADIR/icons
    icon.files += Resources/icons/tradietrakka.png
}




