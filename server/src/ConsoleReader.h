#pragma once

#include <QThread>

class ConsoleReader : public QThread
{
    Q_OBJECT

public:
    explicit ConsoleReader(QObject* parent = nullptr);
    ~ConsoleReader() override;

    void stop();

signals:
    void lineRead(const QString& line);

protected:
    void run() override;
};
