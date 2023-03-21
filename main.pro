TEMPLATE=app
TARGET=window
QT=core gui widgets
SOURCES+=window.cpp \
    main.cpp
HEADERS+=window.hpp
RESOURCES     = application.qrc
LIBS += -lhunspell-1.7

INCLUDEPATH += /usr/share/hunspell
