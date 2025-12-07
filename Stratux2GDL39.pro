QT += core network bluetooth websockets
QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = Stratux2GDL39
TEMPLATE = app

# Tell the compiler to look in the 'include' folder for #include statements
INCLUDEPATH += ./include
DEPENDPATH += ./include

# Install path (optional)
target.path = /home/pi/Stratux2GDL39
INSTALLS += target

# Source files (Assuming these are in the root folder)
SOURCES += \
    main.cpp \
    StreamReader.cpp \
    Translator.cpp

# Header files (Explicitly pointing to the include folder)
HEADERS += \
    include/StreamReader.h \
    include/Translator.h \
    include/StubTypes.h