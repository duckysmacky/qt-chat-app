#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/qqml.h>
#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QTextStream>

#include "ChatWindow.h"
#include "Client.h"

static QFile g_logFile;

/**
 * @brief Custom Qt message handler that writes log messages to a file.
 * @param type The type of the log message.
 * @param context The message context (file, line, etc.).
 * @param message The log message text.
 */
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

/**
 * @brief Main entry point of the Qt chat client application.
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 * @return Application exit code.
 */
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

    qmlRegisterSingletonInstance("client", 1, 0, "Client", &Client::instance());
    qmlRegisterType<ChatWindow>("client", 1, 0, "ChatWindow");

    engine.loadFromModule("client", "Main");

    return QGuiApplication::exec();
}
