INCLUDEPATH += $$PWD

include(mp_MapProcessor/MapProcessor.pri)
include(mp_MapJsonQueue/MapJsonQueue.pri)

FORMS += \
    $$PWD/mp_bulksend.ui

HEADERS += \
    $$PWD/mp_bulksend.h

SOURCES += \
    $$PWD/mp_bulksend.cpp
