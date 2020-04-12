TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_LFLAGS += -pthread
QMAKE_CXXFLAGS += -fgnu-tm
LIBS += -lGL -lGLU -lglut

SOURCES += main.cpp \
    cell.cpp \
    coords.cpp \
    map.cpp \
    groupmanager.cpp \
    drawer.cpp \
    threading.cpp \
    directionvector.cpp \
    distributor.cpp \
    cellbot.cpp \
    program.cpp \
    commands.cpp \
    randomizer.cpp \
    config.cpp

HEADERS += \
    printer.h \
    cell.h \
    coords.h \
    map.h \
    groupmanager.h \
    drawer.h \
    threading.h \
    config.h \
    directionvector.h \
    distributor.h \
    cellbot.h \
    type.h \
    program.h \
    commands.h \
    randomizer.h \
    config.h \
    clustermap.h
