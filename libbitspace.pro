QT       = core network

TARGET = bitspace
TEMPLATE = lib

DEFINES += BITSPACE_LIBRARY

SOURCES += ws.cpp \
    upload/UploadManager.cpp \
    NetworkAccessManager.cpp \
    upload/Upload.cpp

HEADERS += ws.h\
           bitspace_global.h \
    upload/UploadManager.h \
    NetworkAccessManager.h \
    upload/Upload.h
INCLUDEPATH += /usr/include
LIBS += /usr/lib/libqjson.so
