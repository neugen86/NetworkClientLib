#include <QCoreApplication>

#include "Test/Test.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    (new Test())->run();

    return a.exec();
}
