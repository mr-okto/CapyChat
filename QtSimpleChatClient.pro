QT += core network gui widgets

TARGET = QtSimpleChatClient
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS


CONFIG += debug_and_release

SOURCES += \
    clientmain.cpp \
    chatwindow.cpp \
    chatclient.cpp \
    node.cpp \
    utils.cpp \
    bundled_tools.cpp

FORMS += \
    chatwindow.ui

HEADERS += \
    chatwindow.h \
    chatclient.h \
    bundled_tools.h \
    utils.h
