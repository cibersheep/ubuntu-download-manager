include(../common-project-config.pri)
include(../common-vars.pri)

QT       += network dbus systeminfo sql
QT       -= gui
DEFINES += NDEBUG

TARGET = ubuntu-upload-manager-priv
TEMPLATE = lib

DEFINES += UBUNTUUPLOADMANAGERPRIV_LIBRARY

SOURCES += \
    ubuntu/uploads/upload_adaptor.cpp \
    ubuntu/uploads/upload_manager_adaptor.cpp

HEADERS += \
    ubuntu/uploads/upload_adaptor.h \
    ubuntu/uploads/upload_manager_adaptor.h

include(../common-installs-config.pri)

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-common/ -lubuntu-download-manager-common

INCLUDEPATH += $$PWD/../ubuntu-download-manager-common
DEPENDPATH += $$PWD/../ubuntu-download-manager-common

LIBS += -L$$OUT_PWD/../udm-priv-common/ -ludm-priv-common

INCLUDEPATH += $$PWD/../udm-priv-common
DEPENDPATH += $$PWD/../udm-priv-common
