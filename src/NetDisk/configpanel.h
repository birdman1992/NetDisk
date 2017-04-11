#ifndef CONFIGPANEL_H
#define CONFIGPANEL_H

#include <QWidget>

namespace Ui {
class ConfigPanel;
}

class ConfigPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigPanel(QWidget *parent = 0);
    ~ConfigPanel();
    bool configCheck();
    bool configIsFinished();

private slots:
    void on_downloadSet_clicked();

    void on_synccSet_clicked();

    void on_autosync_toggled(bool checked);

    void on_maxTask_activated(int index);

    void on_finish_clicked();

    void on_serverAddr_editingFinished();

    void on_cancel_clicked();

signals:
    void configOver();

private:
    Ui::ConfigPanel *ui;
    bool configFinshed;
    void initStack();
};

#endif // CONFIGPANEL_H
