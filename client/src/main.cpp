#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/qqml.h>

#include "Client.h"

int main(int argc, char *argv[])
{
    const QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection
    );

    qmlRegisterType<Client>("client", 1, 0, "Client");

    engine.loadFromModule("client", "Main");

    return QGuiApplication::exec();
}
