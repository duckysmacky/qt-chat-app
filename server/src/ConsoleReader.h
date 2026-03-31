#pragma once

#include <QThread>

/**
 * @class ConsoleReader
 * @brief Thread for console input reading
 * Emits a signal each time new line is readed from stdin
 */
class ConsoleReader : public QThread
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the ConsoleReader
     */
    explicit ConsoleReader(QObject* parent = nullptr);
    
    /**
     * Destroys the ConsoleReader object
     */
    ~ConsoleReader() override;

    /**
     * @brief Stops the ConsoleReader loop
     */ 
    void stop();

signals:
    /**
     * @brief Emitted when a line is read from the console
     * @param line The input line
     */
    void lineRead(const QString& line);

protected:
    /// @brief Continuously reads input until 'stop' is called
    void run() override;
};
