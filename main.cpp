#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <gameplay.h>

#ifdef _WIN32
#include <QQuickWindow>
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
    QQuickWindow::setSceneGraphBackend("software");
#endif

    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/resources/app.ico"));

    QQmlApplicationEngine engine;

    GamePlay* gamePlay = new GamePlay();
    engine.rootContext()->setContextProperty("GamePlay", gamePlay);

    const QUrl url(QStringLiteral("qrc:/mainwindow.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
