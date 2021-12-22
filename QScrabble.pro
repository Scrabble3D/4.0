QT += quick widgets

CONFIG += c++17
CONFIG += qmltypes

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        board.cpp \
        boardmodel.cpp \
        cubemodel.cpp \
        dictionary.cpp \
        move.cpp \
        main.cpp \
        gameplay.cpp \
        msgmodel.cpp \
        rackmodel.cpp \
        statmodel.cpp

HEADERS += \
        board.h \
        boardmodel.h \
        cubemodel.h \
        dictionary.h \
        move.h \
        gameplay.h \
        letter.h \
        msgmodel.h \
        rackmodel.h \
        statmodel.h

RESOURCES += \
        qml.qrc

#DESTDIR = imports/GamePlay
#TARGET = gameplay

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_NAME = Scrabble
QML_IMPORT_MAJOR_VERSION = 4
QML_IMPORT_MINOR_VERSION = 0
QML_IMPORT_PATH = $$PWD

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
