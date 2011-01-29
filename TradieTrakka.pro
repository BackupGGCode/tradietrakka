# -------------------------------------------------
# Project created by QtCreator 2010-06-01T14:53:02
# -------------------------------------------------
TARGET = TradieTrakka
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

win32{
	CONFIG -= console
	RC_FILE = Resources/icon.rc
}

macx{
    ICON = Resources/icons/tt.icns
}
