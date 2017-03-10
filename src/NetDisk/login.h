#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QString>
#include "netconfig.h"

namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    ~login();

private:
    Ui::login *ui;
    bool wMoveable;
    QPoint dragPosition;
    QString username;
    QString passwd;
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);

signals:
    void winClose();
    void netLogin();

private slots:
    void on_btn_min_clicked();
    void on_btn_set_clicked();
    void on_btn_close_clicked();
    void on_btn_login_clicked();
    void on_isrem_toggled(bool checked);
    void on_isauto_toggled(bool checked);
};

#endif // LOGIN_H
