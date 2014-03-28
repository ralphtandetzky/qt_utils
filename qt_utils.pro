QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QMAKE_CXXFLAGS += -std=c++0x -pedantic

TEMPLATE = lib
CONFIG += staticlib
DEPENDPATH += .
INCLUDEPATH += . ..

# Input
HEADERS += event_filter.h \
           exception_handling.h \
           exception_handling_application.h \
           gui_property_sheet.h \
           gui_user_parameter.h \
           invoke_in_thread.h \
           loop_thread.h \
           serialize_props.h
SOURCES += exception_handling.cpp \
           gui_property_sheet.cpp \
           gui_user_parameter.cpp \
           serialize_props.cpp
