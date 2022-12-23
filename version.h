#pragma once

#include <QString>
#include <QStringLiteral>

#ifdef Q_OS_Windows
    const QString InstFileName = "Scrabble3D_Windows.exe"
#elif defined(Q_OS_Android)
    const QString InstFileName = "Scrabble3D_Android.apk"
#elif defined(Q_OS_MACOS)
    const QString InstFileName = "Scrabble3D_macOS.dmg"
#elif defined(Q_OS_LINUX)
    const QString InstFileName = ""; //pointless to download a binary under Linux
//    const QString InstFileName = "Scrabble3D_Linux.bin" // static build with installer
#endif

class version
{

#define VERSION_MAJOR 4
#define VERSION_MINOR 0
#define VERSION_BUILD 1 //FIXME!!!!

public:
    version() {};

    static QString current() {
        return QString::number(VERSION_MAJOR) + "." +
               QString::number(VERSION_MINOR) + "." +
               QString::number(VERSION_BUILD);
    }
    static QString toString(const int aValue)
    {
        int aMajor, aMinor, aBuild;
        int temp;

        if (aValue == -1)
            return "";

        temp = aValue;
        aMajor = temp / 100000;
        temp = temp-aMajor*100000;
        aMinor = temp / 1000;
        temp = temp-aMinor*1000;
        aBuild = temp;
        return QString::number(aMajor)+'.'+QString::number(aMinor)+'.'+QString::number(aBuild);
    }
    static int fromString(const QString aVersion)
    {
        int aMajor, aMinor, aBuild;
        QString temp;
        int pos1 = aVersion.indexOf(".");
        int pos2 = aVersion.indexOf(".", pos1 + 1);
        temp = aVersion.left( pos1);
        aMajor = temp.toInt();
        temp = aVersion.mid(pos1 + 1, pos2 - (pos1 + 1));
        aMinor = temp.toInt();
        temp = aVersion.last( aVersion.length() - (pos2 + 1));
        aBuild = temp.toInt();
        return aMajor * 100000 + aMinor * 1000 + aBuild;
    }
};
