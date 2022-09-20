#pragma once

#include <QDir>
#include <QStandardPaths>

class config
{
private:
    static const QDir configPath() {
        QDir aDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/4";
        if (!aDir.exists())
            QDir().mkpath(aDir.path());
        return aDir.path();
    }
    static const QDir tempPath() {
        return QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    }

public:
    static QDir path() {
        return configPath();
    }
    static QString temp(QString fileName) {
        return tempPath().path() + "/" + fileName;
    }
    static QString ini() {
        return configPath().path() + "/Scrabble3D.ini";
    }
    static QString conf() {
        return configPath().path() + "/Scrabble3D.conf";
    }
    static QString file(QString fileName) {
        return configPath().path() + "/" + fileName;
    }
};
