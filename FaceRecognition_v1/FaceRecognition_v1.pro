#-------------------------------------------------
#
# Project created by QtCreator 2022-05-16T10:09:54
#
#-------------------------------------------------

QT       += core gui
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FaceRecognition_v1
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    usersmanage.cpp \
    verification.cpp \
    addusers.cpp \
    arcface.cpp \
    subthread.cpp \
    about.cpp \
    liveface.cpp \
    mtcnn.cpp \
    retinaface.cpp

HEADERS  += mainwidget.h \
    usersmanage.h \
    verification.h \
    addusers.h \
    arcface.h \
    subthread.h \
    about.h \
    liveface.h \
    mtcnn.h \
    retinaface.h

FORMS    += mainwidget.ui \
    usersmanage.ui \
    verification.ui \
    addusers.ui \
    about.ui

RESOURCES += \
    resources.qrc

#opencv
INCLUDEPATH += $$PWD/opencv86/include
LIBS += -L$$PWD/opencv86/lib -lopencv_videoio -latomic -lopencv_objdetect
DEPENDPATH += $$PWD/opencv86/lib

#ncnn
INCLUDEPATH += $$PWD/libncnn/include
LIBS += -L$$PWD/libncnn -lncnn
