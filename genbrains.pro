TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_LFLAGS += -pthread
LIBS += -lGL -lGLU -lglut

include(sources/sources.pri)
include(headers/headers.pri)
