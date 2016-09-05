TEMPLATE = app

DEPENDPATH += .
INCLUDEPATH += .

QT += network widgets

RESOURCES += images.qrc

FORMS   += MainWindow.ui 

HEADERS += MainWindow.h \ 
    simplecrypt.h \
    clefia.h

SOURCES += \
           main.cpp \
           MainWindow.cxx \
    simplecrypt.cpp \
    clefia.cpp
