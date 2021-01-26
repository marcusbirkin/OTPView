#-------------------------------------------------
#
# Project created by QtCreator 2019-07-03T20:35:29
#
#-------------------------------------------------

QT       += core gui network charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OTPView
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

include($$PWD/../version.pri)
include($$PWD/../libs/libs.pri)

RC_FILE = res/OTPView.rc
RESOURCES += res/OTPView.qrc

INCLUDEPATH += src

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/settings.cpp \
    src/settingsdialog.cpp \
    src/producerwindow.cpp \
    src/groupselectiondialog.cpp \
    src/groupwindow.cpp \
    src/pointselectiondialog.cpp \
    src/systemselectiondialog.cpp \
    src/systemwindow.cpp \
    src/models/pointstablemodel.cpp \
    src/models/componentsmodel.cpp \
    src/models/systemmodel.cpp \
    src/widgets/linechart.cpp \
    src/widgets/systemspinbox.cpp \
    src/widgets/groupspinbox.cpp \
    src/widgets/pointspinbox.cpp \
    src/widgets/spacialspinbox.cpp \
    src/widgets/scalespinbox.cpp \
    src/widgets/priorityspinbox.cpp

HEADERS += \
    src/mainwindow.h \
    src/settings.h \
    src/settingsdialog.h \
    src/producerwindow.h \
    src/groupselectiondialog.h \
    src/groupwindow.h \
    src/pointselectiondialog.h \
    src/systemselectiondialog.h \
    src/systemwindow.h \
    src/models/pointstablemodel.h \
    src/models/componentsmodel.h \
    src/models/systemmodel.h \
    src/widgets/linechart.h \
    src/widgets/systemspinbox.h \
    src/widgets/groupspinbox.h \
    src/widgets/pointspinbox.h \
    src/widgets/spacialspinbox.h \
    src/widgets/scalespinbox.h \
    src/widgets/priorityspinbox.h

FORMS += \
    src/mainwindow.ui \
    src/settingsdialog.ui \
    src/producerwindow.ui \
    src/groupselectiondialog.ui \
    src/groupwindow.ui \
    src/pointselectiondialog.ui \
    src/systemselectiondialog.ui \
    src/systemwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
