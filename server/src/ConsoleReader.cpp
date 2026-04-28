#include "ConsoleReader.h"

#include <QTextStream>

ConsoleReader::ConsoleReader(QObject* parent)
    : QThread(parent)
{
}

ConsoleReader::~ConsoleReader()
{
    stop();
}

void ConsoleReader::stop()
{
    requestInterruption();
    wait();
}

void ConsoleReader::run()
{
    QTextStream in(stdin);

    while (!isInterruptionRequested())
    {
        const QString line = in.readLine();
        if (line.isNull())
            break;
        emit lineRead(line);
    }
}