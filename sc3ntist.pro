include(common.pri)

QT += core gui widgets

TARGET = sc3ntist
TEMPLATE = app

INCLUDEPATH += src/gui

SOURCES += $$files(src/gui/*.cpp, true)
SOURCES += $$files(src/gui/*.c, true)
HEADERS += $$files(src/gui/*.h, true)
FORMS   += $$files(src/gui/*.ui, true)