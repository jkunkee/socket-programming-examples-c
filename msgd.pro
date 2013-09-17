TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += client.cc \
    infinibuf.cc \
    msgd.cc \
    msgdclient.cc \
    logger.cc \
    msgserver.cc \
    socket.cc

HEADERS += \
    client.h \
    infinibuf.h \
    proto.h \
    logger.h \
    msgserver.h \
    socket.h


