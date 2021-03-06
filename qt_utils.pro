QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QMAKE_CXXFLAGS += -std=c++11 -pedantic

TEMPLATE = lib
CONFIG += staticlib create_prl c++11 link_prl
DEPENDPATH += . ../cpp_utils/
INCLUDEPATH += ..

# Input
HEADERS += event_filter.h \
           exception_handling.h \
           exception_handling_application.h \
           gui_property_sheet.h \
           gui_user_parameter.h \
           invoke_in_thread.h \
           loop_thread.h \
           serialize_props.h \
    gui_progress_widget.h \
    gui_progress_manager.h \
    event_handling_graphics_item.h

SOURCES += exception_handling.cpp \
           gui_property_sheet.cpp \
           gui_user_parameter.cpp \
           serialize_props.cpp \
    gui_progress_widget.cpp \
    gui_progress_manager.cpp

LIBS += -L../cpp_utils -lcpp_utils \

FORMS += \
    gui_progress_widget.ui

RESOURCES += \
    qt_utils.qrc
