QT += core network bluetooth websockets
QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = Stratux2GDL39
TEMPLATE = app

# Adjust this path if you want to install it elsewhere
target.path = /home/pi/Stratux2GDL39
INSTALLS += target

SOURCES += \
    main.cpp \
    StreamReader.cpp \
    Translator.cpp

HEADERS += \
    StreamReader.h \
    Translator.h \
    StubTypes.h