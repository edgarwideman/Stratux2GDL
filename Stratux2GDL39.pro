#-------------------------------------------------
#
# Project created by QtCreator 2018-01-04T19:26:13
#
#-------------------------------------------------
QT += core websockets network bluetooth
QT -= gui
CONFIG += c++11
TARGET = Stratux2GDL39
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
INCLUDEPATH += ./include
VPATH += ./include
target.path = /home/pi/Stratux2GDL39
INSTALLS += target
SOURCES += \
    main.cpp \
    StreamReader.cpp \
    TrafficMath.cpp \
    Translator.cpp
HEADERS += \
    StratuxStreams.h \
    StreamReader.h \
    Translator.h