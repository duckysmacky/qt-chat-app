#include "ConsoleReader.h"

#include <QTextStream>

/**
 * @brief Constructs the ConsoleReader thread
 */
ConsoleReader::ConsoleReader(QObject* parent)
    : QThread(parent)
{
}

/**
 * @brief Stops the thread execution
 * Requests interruption and waits for the thread to finish.
 */
ConsoleReader::~ConsoleReader()
{
    stop();
}

/**
 * @brief Requests the thread to stop.
 * 
 */
void ConsoleReader::stop()
{
    requestInterruption();
    wait();
}

/**
 * @brief Continuously reads lines from stdin end emits lineRead()
 * Stops when interruption is requested or input ends
 * @warning Blocking call (readLine). Thread may not stop immediately
 */
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
