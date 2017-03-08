#include "mainwidget.h"
#include <QApplication>
#include <qfont.h>
#include "netconfig.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont font;
    font.setFamily("微软雅黑");
    font.setPointSize(9);
    a.setFont(font);
    netConf = new NetConfig;
    MainWidget w;
    w.show();

    return a.exec();
}
