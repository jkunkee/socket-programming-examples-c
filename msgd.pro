TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += client.cc \
    infinibuf.cc \
    msgd.cc \
    msgdclient.cc \
    server.cc

HEADERS += \
    client.h \
    infinibuf.h \
    proto.h \
    server.h


