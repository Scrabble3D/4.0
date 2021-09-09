#include <QApplication>
//#include <QQmlApplicationEngine>
#include <QtQuick3D/qquick3d.h>
//#include <QQmlComponent>

//#include <QQmlContext>
#include <gameplay.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QSurfaceFormat::setDefaultFormat(QQuick3D::idealSurfaceFormat());

    QQmlApplicationEngine engine;

    qmlRegisterUncreatableMetaObject(LetterState::staticMetaObject, "letterstate", 1, 0, "LetterState", "Error");

//    QQmlComponent component(&engine, QUrl("qrc:qml.qrc/"));
//    auto *rack = qobject_cast<QRack *>(component.create());

    QGamePlay* GamePlay = new QGamePlay();
    engine.rootContext()->setContextProperty("GamePlay", GamePlay);

//    qRegisterMetaType<Letter>();
    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
