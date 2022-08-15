#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <gameplay.h>

// https://bugreports.qt.io/browse/QTBUG-102634
// use "set QT_D3D_ADAPTER_INDEX=1" on Windows
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/resources/app.ico"));

    QQmlApplicationEngine engine;

    GamePlay* _GamePlay = new GamePlay(&engine);
    engine.rootContext()->setContextProperty("GamePlay", _GamePlay);

    const QUrl url(QStringLiteral("qrc:/mainwindow.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
