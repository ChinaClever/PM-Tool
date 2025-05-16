
INCLUDEPATH += $$PWD

FORMS += \
    $$PWD/ip_jsondata.ui

HEADERS += \
    $$PWD/ip_jsondata.h \
    $$PWD/ip_sendjson.h

SOURCES += \
    $$PWD/ip_jsondata.cpp \
    $$PWD/ip_sendjson.cpp

include (RanNumGgen/RanNumGgen.pri)
include (data_cal/data_cal.pri)
include (ip_BulkSend/ip_BulkSend.pri)
#INCLUDEPATH += $$PWD/ip/jsondata DEPENDPATH += $$PWD/ip/jsondata
