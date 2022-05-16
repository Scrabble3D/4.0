#pragma once

#include <QString>

#define VERSION_MAJOR 4
#define VERSION_MINOR 0
#define VERSION_BUILD 0

static QString versionString() {
    return QString::number(VERSION_MAJOR) + "." +
           QString::number(VERSION_MINOR) + "." +
           QString::number(VERSION_BUILD);
}
