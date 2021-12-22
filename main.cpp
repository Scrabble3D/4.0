#include <QApplication>
#include <QIcon>
#include <gameplay.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/resources/app.ico"));

    QQmlApplicationEngine engine;

    qmlRegisterUncreatableMetaObject(LetterState::staticMetaObject, "letterstate", 1, 0, "LetterState", "Error");

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
