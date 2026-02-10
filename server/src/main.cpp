#include <QCoreApplication>
#include <QTextStream>

#include "TcpServer.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
		
	TcpServer server;
	server.start();

    return app.exec();
}
