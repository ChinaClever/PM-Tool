INCLUDEPATH += $$PWD
CONFIG += c++17
include(mp_data/mp_data.pri)
include(data_struct/data_struct.pri)
include(mp_bulksend/mp_bulksend.pri)

HEADERS += \
    $$PWD/mp_globals.h \
    $$PWD/mp_toJson.h

SOURCES += \
    $$PWD/mp_globals.cpp \
    $$PWD/mp_toJson.cpp
