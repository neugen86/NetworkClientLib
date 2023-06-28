#include <QCoreApplication>

#include "Test/Test.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    auto test = new Test();
    test->run();

    return a.exec();
}
