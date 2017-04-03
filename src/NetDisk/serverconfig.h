#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include <QWidget>

namespace Ui {
class ServerConfig;
}

class ServerConfig : public QWidget
{
    Q_OBJECT

public:
    explicit ServerConfig(QWidget *parent = 0);
    ~ServerConfig();

private slots:
    void on_btn_conf_clicked();

    void on_btn_cancel_clicked();

private:
    Ui::ServerConfig *ui;
};

#endif // SERVERCONFIG_H
