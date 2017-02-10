#include "pathbox.h"

PathBox::PathBox(QWidget *parent):QComboBox(parent)
{
    fileDir = QString("/");
    connect(this, SIGNAL(activated(QString)), this, SLOT(pathTrans(QString)));
}

void PathBox::setBoxPath(QList<QString *> list)
{
    if(list.isEmpty())
        return;

    if(list.count() == 1)
    {
        fileDir = QString("/");
        return;
    }

    int i;

    for(i=1; i<list.count(); i++)
    {
        fileDir += *(list.at(i)) + "/";
    }

    return;
}

void PathBox::pathTrans(const QString &text)
{
    emit pathSelected(fileDir+text);
}
