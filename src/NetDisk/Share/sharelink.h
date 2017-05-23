#ifndef SHARELINK_H
#define SHARELINK_H

#include <QWidget>
#include <qpainter.h>
#include <QPaintEvent>
#include <QShowEvent>
#include <QLabel>

namespace Ui {
class ShareLink;
}

class ShareLink : public QWidget
{
    Q_OBJECT

public:
    explicit ShareLink(QString link, QString passwd, QWidget *parent = 0);
    ~ShareLink();

private slots:
    void on_close_clicked();
    void on_copy_clicked();
    void tipTimeOut();
    void showTipTimeout();

private:
    Ui::ShareLink *ui;
    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent*);
    void showTips(QString tips);
    QString share_link;
    QString share_passwd;
    QLabel* win_tips;
    bool waitForDelete;
};

#endif // SHARELINK_H
