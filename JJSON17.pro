QT += testlib
QT -= gui

CONFIG += c++17
CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_qjsoncompatibility.cpp \
    ../jjson17/jjson17.cpp

HEADERS += \
    ../jjson17/jjson17.h

INCLUDEPATH += ../jjson17
