#include <QCoreApplication>
#include <QTextStream>
#include <QFile>
#include <QTextStream>
#include <QStringList>

#include "TcpServer.h"
#include "Database.h"

void load_env_file(const QString& file_path) 
{
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream file_stream(&file);
    while (!file_stream.atEnd()) 
	{
        QString line = file_stream.readLine().trimmed();
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
	load_env_file(".env");

	Database& db = Database::instance();
	
	TcpServer server;
	server.start();

    return app.exec();
}
