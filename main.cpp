#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>

#include <gameplay.h>
// TODO: main: Android splash screen / app icon

// https://bugreports.qt.io/browse/QTBUG-102634
// use "set QT_D3D_ADAPTER_INDEX=1" on Windows
int main(int argc, char *argv[])
{
    QApplication scrabble3D(argc, argv);
    scrabble3D.setApplicationName("Scrabble3D");
    // this changes the path for the settings
//    scrabble3D.setOrganizationName("Scrabble3D"); // .config/Scrabble3D
//    scrabble3D.setOrganizationDomain("4"); // .config/Scrabble3D/4 - to separate from v3 ini's
//    QSettings::setDefaultFormat(QSettings::IniFormat); // applies to qml as well; but "location" does not work

    // macOS messes up with native main menu
    scrabble3D.setAttribute(Qt::AA_DontUseNativeMenuBar);

    QThread::currentThread()->setObjectName("Main Thread");

    scrabble3D.setWindowIcon(QIcon(":/resources/app.ico"));

    QQmlApplicationEngine engine;

    //qml -> qt connections
    GamePlay* _GamePlay( new GamePlay(&engine) );
    engine.rootContext()->setContextProperty("GamePlay", _GamePlay);

    const QUrl url(QStringLiteral("qrc:/mainwindow.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &scrabble3D, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);
    return scrabble3D.exec();
}
