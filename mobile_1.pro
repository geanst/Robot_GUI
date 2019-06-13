#-------------------------------------------------
#
# Project created by QtCreator 2019-04-14T23:34:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mobile_1
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    logger.cpp \
        main.cpp \
        mainwindow.cpp \
    robotcursor.cpp \
    grid.cpp \
    cell.cpp \
    robotpioneer.cpp \
    thcompass.cpp

HEADERS += \
    logger.h \
        mainwindow.h \
    robotcursor.h \
    grid.h \
    cell.h \
    robotpioneer.h \
    thcompass.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#Aria

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../usr/local/Aria/lib/release/ -lAria
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../usr/local/Aria/lib/debug/ -lAria
else:unix: LIBS += -L$$PWD/../../../../../../../usr/local/Aria/lib/ -lAria

INCLUDEPATH += $$PWD/../../../../../../../usr/local/Aria/include
DEPENDPATH += $$PWD/../../../../../../../usr/local/Aria/include
