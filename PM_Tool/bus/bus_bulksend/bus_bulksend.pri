INCLUDEPATH += $$PWD
CONFIG += c++17

include(MapJsonQueue/MapJsonQueue.pri)
include(MapProcessor/MapProcessor.pri)

FORMS += \
    $$PWD/busbulk.ui

HEADERS += \
    $$PWD/busbulk.h

SOURCES += \
    $$PWD/busbulk.cpp
