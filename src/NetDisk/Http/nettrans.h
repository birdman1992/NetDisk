#ifndef NETTRANS_H
#define NETTRANS_H

#include <QObject>
#include <QThread>

class netTrans : public QThread
{
    Q_OBJECT
public:
    explicit netTrans(QObject *parent = 0);
    void run();
};

#endif // NETTRANS_H
