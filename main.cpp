#include <QCoreApplication>
#include "spambot.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Bot w;
    return a.exec();
}
