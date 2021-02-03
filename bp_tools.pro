#-------------------------------------------------
#
# Project created by QtCreator 2020-12-28T12:58:05
#
#-------------------------------------------------

QT       += core gui serialport charts qml quick quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

requires(qtConfig(combobox))

TARGET = BlackPillTools
TEMPLATE = app


INCLUDEPATH +=.
DEPENDPATH +=.
#TRANSLATIONS += "$$TARGET"_en.ts
#        "$$TARGET"_de.ts \
#        "$$TARGET"_fr.ts



#Application version
VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_BUILD = 22

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

unix: {
    outFileName = "ver.h"
    system(echo $$TARGET-$$VERSION > $$outFileName)
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#CONFIG += c++14


SOURCES += \
        compDialog.cpp \
        graf.cpp \
        itDialog.cpp \
        main.cpp \
        mainwindow.cpp \
        settingsdialog.cpp

HEADERS += \
        bnumber.h \
        compDialog.h \
        defs.h \
        graf.h \
        itDialog.h \
        mainwindow.h \
        settingsdialog.h \
        version.h

FORMS += \
        compDialog.ui \
        itDialog.ui \
        mainwindow.ui \
        settingsdialog.ui

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc

DISTFILES += \
    compass.qml





