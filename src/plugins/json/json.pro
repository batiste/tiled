include(../plugin.pri)

DEFINES += JSON_LIBRARY

SOURCES += jsonplugin.cpp
HEADERS += jsonplugin.h

LIBS += -L/usr/lib
LIBS += -lqjson

