QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
# PM-Tool.pro

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(ip/ip.pri)
include(common/common.pri)
include(mainPage/mainPage.pri)

SOURCES += \
    ip_navarwid.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ip_navarwid.h \
    mainwindow.h

FORMS += \
    ip_navarwid.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images/image.qrc

DISTFILES += \
    images/back.jpg \
    images/box_back.jpg \
    images/logo.jpg \
    images/title_back.jpg
