TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    msgd.cc \
    msg.cc \
    infinibuf.cc \
    logger.cc \
    socket.cc \
    msgserver.cc \
    msgclient.cc

HEADERS += \
    infinibuf.h \
    logger.h \
    socket.h \
    proto.h \
    msgserver.h \
    msgclient.h


