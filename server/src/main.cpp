#include <QCoreApplication>
#include <QTextStream>
#include <QFile>
#include <QStringList>

#include "TcpServer.h"
#include "Database.h"

void loadEnvFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

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
	loadEnvFile("../../.env");

	Database& db = Database::instance();
	if (!db.connect())
	{
		qFatal() << "A connection to the database is required for the sever to start";
		return 1;
	}

	TcpServer server;
	if (!server.start())
	{
		qFatal() << "An error occurred when starting the server";
		return 1;
	}

  return QCoreApplication::exec();
}
