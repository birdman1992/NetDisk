#ifndef SYNCLIST_H
#define SYNCLIST_H

#include <QWidget>

namespace Ui {
class syncList;
}

class syncList : public QWidget
{
    Q_OBJECT

public:
    explicit syncList(QWidget *parent = 0);
    ~syncList();

private:
    Ui::syncList *ui;
};

#endif // SYNCLIST_H
