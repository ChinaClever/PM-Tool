INCLUDEPATH += $$PWD

include(backcolour/backcolour.pri)
include(datapacket/datapacket.pri)
include(dbcom/dbcom.pri)
include(sqlcom/sqlcom.pri)
include(msgcom/msgcom.pri)
include(cfgcom/cfgcom.pri)
include(excel/excel.pri)
include(logcom/logcom.pri)
include(SettingsHelper/SettingsHelper.pri)

HEADERS += \
    $$PWD/common.h

SOURCES += \
    $$PWD/common.cpp
