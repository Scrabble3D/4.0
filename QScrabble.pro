QT += quick widgets concurrent

CONFIG += c++17
CONFIG += qmltypes

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.2.0

ICON = resources/Scrabble3D.icns
RC_ICONS = resources/app.ico

SOURCES += \
        board.cpp \
        boardmodel.cpp \
        computemove.cpp \
        cubemodel.cpp \
        dictionary.cpp \
        dictionarymodel.cpp \
        downloadmanager.cpp \
        gamecoursemodel.cpp \
        gameplay_network.cpp \
        locmodel.cpp \
        move.cpp \
        main.cpp \
        gameplay.cpp \
        msgmodel.cpp \
        network.cpp \
        playersmodel.cpp \
        rackmodel.cpp \
        remotegamesmodel.cpp \
        zip.c

HEADERS += \
        board.h \
        boardmodel.h \
        computemove.h \
        configpath.h \
        cubemodel.h \
        dictionary.h \
        dictionarymodel.h \
        downloadmanager.h \
        gamecoursemodel.h \
        locmodel.h \
        move.h \
        gameplay.h \
        letter.h \
        msgmodel.h \
        network.h \
        playersmodel.h \
        rackmodel.h \
        remotegamesmodel.h \
        version.h \
        zip.h

RESOURCES += \
        qml.qrc \

TARGET = Scrabble3D

QML_IMPORT_NAME = Scrabble3D
QML_IMPORT_MAJOR_VERSION = 4
QML_IMPORT_MINOR_VERSION = 0
QML_IMPORT_PATH = $$PWD

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
android: include($$PWD/android-sdk/android_openssl/openssl.pri)

# android-sdk created as soft link to the actual opensssl path
contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/android-sdk/android_openssl/ssl_3/arm64-v8a/libcrypto_3.so \
        $$PWD/android-sdk/android_openssl/ssl_3/arm64-v8a/libssl_3.so

    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/android-sdk/android_openssl/ssl_3/armeabi-v7a/libcrypto_3.so \
        $$PWD/android-sdk/android_openssl/ssl_3/armeabi-v7a/libssl_3.so

    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml
