#include "src/include/protochatmainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ProtochatMainWindow w;
    w.show();
    return a.exec();
}
