#ifndef TRANSLIST_H
#define TRANSLIST_H

#include <QWidget>

namespace Ui {
class TransList;
}

class TransList : public QWidget
{
    Q_OBJECT

public:
    explicit TransList(QWidget *parent = 0);
    ~TransList();

private:
    Ui::TransList *ui;
};

#endif // TRANSLIST_H
