QT       = core network

TARGET = bitspace
TEMPLATE = lib

DEFINES += BITSPACE_LIBRARY

SOURCES += ws.cpp \
    upload/Upload.cpp \
    NetworkAccessManager.cpp

HEADERS += ws.h\
           bitspace_global.h \
    upload/Upload.h \
    NetworkAccessManager.h
INCLUDEPATH += /usr/include
LIBS += /usr/lib/libqjson.so
