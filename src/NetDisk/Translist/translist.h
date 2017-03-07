#ifndef TRANSLIST_H
#define TRANSLIST_H

#include <QWidget>
#include "Translist/bardelegata.h"

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
    TransModel* transModel;
    int transNum;
};

#endif // TRANSLIST_H
