# #####################################################################
# Automatically generated by qmake (2.01a) Sun Nov 1 16:24:51 2009
# #####################################################################
TEMPLATE = lib
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += common.h \
    library_extragui_init.h \
    qt_accessors.h
SOURCES +=  library_extragui_init.cpp \
    qt_accessors.cpp
TARGET = ar2tech_gui
QT += xml
CONFIG += plugin
INCLUDEPATH += $(QTDIR)/tools/designer/interfaces
contains( TEMPLATE, lib ) { 
    DESTDIR = $$GSTLAPPLI_HOME/lib/$$CUR_PLATFORM
    OBJECTS_DIR = $$GSTLAPPLI_HOME/lib/$$CUR_PLATFORM/obj/extra/gui
}
contains( TEMPLATE, app ) { 
    DESTDIR += $$GSTLAPPLI_HOME/bin/$$CUR_PLATFORM
    OBJECTS_DIR = $$GSTLAPPLI_HOME/bin/$$CUR_PLATFORM/obj/extra/gui
}
include( $$GSTLAPPLI_HOME/config.qmake )
exists( MMakefile ):include( MMakefile )
