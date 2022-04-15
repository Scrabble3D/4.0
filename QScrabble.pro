QT += quick widgets

CONFIG += c++17
CONFIG += qmltypes

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.2.0

ICON = resources/Scrabble3D.icns

SOURCES += \
        board.cpp \
        boardmodel.cpp \
        computemove.cpp \
        cubemodel.cpp \
        dictionary.cpp \
        downloadmanager.cpp \
        gamecoursemodel.cpp \
        move.cpp \
        main.cpp \
        gameplay.cpp \
        msgmodel.cpp \
        rackmodel.cpp \
        zip.c

HEADERS += \
        board.h \
        boardmodel.h \
        computemove.h \
        cubemodel.h \
        defines.h \
        dictionary.h \
        downloadmanager.h \
        gamecoursemodel.h \
        move.h \
        gameplay.h \
        letter.h \
        msgmodel.h \
        rackmodel.h \
        zip.h

RESOURCES += \
        qml.qrc

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
android: include(/opt/Android/android_openssl/openssl.pri)

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        /opt/Android/android_openssl/latest/arm/libcrypto_1_1.so \
        /opt/Android/android_openssl/latest/arm/libssl_1_1.so \
        /opt/Android/android_openssl/latest/arm64/libcrypto_1_1.so \
        /opt/Android/android_openssl/latest/arm64/libssl_1_1.so \
        /opt/Android/android_openssl/latest/x86/libcrypto_1_1.so \
        /opt/Android/android_openssl/latest/x86/libssl_1_1.so \
        /opt/Android/android_openssl/latest/x86_64/libcrypto_1_1.so \
        /opt/Android/android_openssl/latest/x86_64/libssl_1_1.so
}
