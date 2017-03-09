#ifndef LOADING_H
#define LOADING_H

#include <QWidget>
#include <QTimer>
#include <QString>

namespace Ui {
class loading;
}

class loading : public QWidget
{
    Q_OBJECT

public:
    explicit loading(QWidget *parent = 0);
    ~loading();
    void reloadStart();
    void reloadStop();

private:
    Ui::loading *ui;
    unsigned int count;
    QTimer tReload;
    QString str;

signals:
    void reload();

private slots:
    void reloadTimeout();
};

#endif // LOADING_H
