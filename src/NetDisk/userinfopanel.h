#ifndef USERINFOPANEL_H
#define USERINFOPANEL_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <Http/nethttp.h>

namespace Ui {
class UserInfoPanel;
}

class UserInfoPanel : public QWidget
{
    Q_OBJECT

public:
    explicit UserInfoPanel(QWidget *parent = 0);
    ~UserInfoPanel();

private:
    Ui::UserInfoPanel *ui;
    void paintEvent(QPaintEvent *);

    QString sizeToByte(quint64 fileSize);
public slots:
    void userinfoUpdate(UserInfo);
};

#endif // USERINFOPANEL_H
