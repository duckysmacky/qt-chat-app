#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/qqml.h>
#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QTextStream>

#include "Client.h"

static QFile g_logFile;

void handleLogMessage(const QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    static QMutex logMutex;
    QMutexLocker lock(&logMutex);

    if (!g_logFile.isOpen()) return;

    QString level;
    switch (type) {
        case QtDebugMsg: level = "DEBUG"; break;
        case QtInfoMsg: level = "INFO"; break;
        case QtWarningMsg: level = "WARN"; break;
        case QtCriticalMsg: level = "CRIT"; break;
        case QtFatalMsg: level = "FATAL"; break;
    }

    QTextStream out(&g_logFile);
    out << QDateTime::currentDateTime().toString(Qt::ISODate)
        << " [" << level << "] "
        << message;

    if (context.file && context.line > 0)
        out << " (" << context.file << ":" << context.line << ")";

    out << "\n";
    out.flush();

    if (type == QtFatalMsg)
        abort();
}

int main(int argc, char *argv[])
{
    const QGuiApplication app(argc, argv);

    g_logFile.setFileName("client.log");
    g_logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);

    qInstallMessageHandler(handleLogMessage);

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
