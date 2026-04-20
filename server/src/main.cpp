#include <QCoreApplication>
#include <QProcessEnvironment>
#include <QTextStream>
#include <QFile>
#include <QStringList>

#include "Server.h"
#include "Database.h"

void loadEnvFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
	    qCritical() << "Unable to open .env file:" << file.errorString();
    	return;
    }

    QTextStream fileStream(&file);
    while (!fileStream.atEnd())
	{
        QString line = fileStream.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#')) continue;

        QStringList parts = line.split('=');
        if (parts.size() >= 2) 
		{
            QString key = parts.at(0).trimmed();
            QString value = parts.at(1).trimmed();

            qputenv(key.toLocal8Bit(), value.toLocal8Bit());
        }
    }
}

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);
	loadEnvFile(".env");

	Database& db = Database::instance();
	if (!db.connect())
	{
		qFatal("%s", "A connection to the database is required for the sever to start");
	}

	if (!db.init())
	{
		qFatal("%s", "Failed to initialize the database");
	}

	const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	const uint16_t port = env.contains("PORT") ? env.value("PORT").toUShort() : 8080;

	Server& server = Server::instance();
	if (!server.start(port))
	{
		qFatal("%s", "An error occurred when starting the server");
	}

	return QCoreApplication::exec();
}
