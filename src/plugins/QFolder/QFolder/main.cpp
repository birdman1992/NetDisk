#include "qfolder.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFolder w;
    w.show();

    return a.exec();
}
