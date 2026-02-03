INCLUDEPATH += $$PWD

FORMS += \
    $$PWD/mp_mainpage.ui \
    $$PWD/seripport.ui

HEADERS += \
    $$PWD/mainpage_globals.h \
    $$PWD/mp_mainpage.h \
    $$PWD/seripport.h

SOURCES += \
    $$PWD/mainpage_globals.cpp \
    $$PWD/mp_mainpage.cpp \
    $$PWD/seripport.cpp

include(mp_serSet/mp_serSet.pri)
