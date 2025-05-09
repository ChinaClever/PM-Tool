
INCLUDEPATH += $$PWD

FORMS += \
    $$PWD/ip_jsondata.ui

HEADERS += \
    $$PWD/ip_jsondata.h

SOURCES += \
    $$PWD/ip_jsondata.cpp

include (RanNumGgen/RanNumGgen.pri)
include (data_cal/data_cal.pri)
#INCLUDEPATH += $$PWD/ip/jsondata DEPENDPATH += $$PWD/ip/jsondata
