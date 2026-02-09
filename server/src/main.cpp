#include <QCoreApplication>
#include <QTextStream>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    QTextStream cout(stdout);

    cout << "Hello, world!\n";
    cout.flush();

    return app.exec();
}
