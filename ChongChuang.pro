#-------------------------------------------------
#
# Project created by QtCreator 2019-01-03T13:49:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChongChuang
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    settingdialog.cpp \
    modelsetdialog.cpp \
    mediator.cpp \
    cryptdialog.cpp \
    normalcontroldialog.cpp \
    checkprocess.cpp

HEADERS += \
        mainwindow.h \
    settingdialog.h \
    debuginfo.h \
    global.h \
    modelsetdialog.h \
    mediator.h \
    excv.h \
    cameras.h \
    MSerialsCV.h \
    common.h \
    preferences.h \
    cryptdialog.h \
    normalcontroldialog.h \
    mdxf.h \
    massert.h \
    checkprocess.h

FORMS += \
        mainwindow.ui \
    settingdialog.ui \
    modelsetdialog.ui \
    cryptdialog.ui \
    normalcontroldialog.ui


INCLUDEPATH +=  ../third_party/opencv4.0.1/include \
                ../third_party/opencv4.0.1/include/opencv2 \
                ../third_party/halcon12 \
                ../third_party/halcon12/halconcpp

INCLUDEPATH += /include/


#使用openmp进行并且处理
QMAKE_CXXFLAGS+=/openmp
#提升线程优先级
QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"

#CONFIG += console

RC_FILE += app.rc

