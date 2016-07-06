QT += core
QT -= gui

CONFIG += c++11

TARGET = parser_gcode
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    parser.cpp \
    g00.cpp \
    g01.cpp \
    g02.cpp \
    figure.cpp \
    output.cpp \
    pause.cpp \
    input.cpp \
    macro.cpp

HEADERS += \
    parser.h \
    deplacement.h \
    g00.h \
    g01.h \
    g02.h \
    ligne.h \
    figure.h \
    output.h \
    pause.h \
    input.h \
    macro.h
