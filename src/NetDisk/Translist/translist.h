#ifndef TRANSLIST_H
#define TRANSLIST_H

#include <QWidget>
#include <QTimer>
#include <QList>
#include "Translist/bardelegata.h"
#include "Http/nettrans.h"

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
    QTimer tProgress;
    TransModel* transModel;
    QList<netTrans*> taskList;
    int transNum;
    int progress;
    int colIndex_speed;
    int colIndex_state;
    int colIndex_progress;

    QString sizeofbytes(quint64 fsize);

private slots:
    void progressCheck();

public slots:
    void newTask(netTrans*);
};

#endif // TRANSLIST_H
