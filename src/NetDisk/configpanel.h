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

private slots:
    void on_downloadSet_clicked();

    void on_synccSet_clicked();

    void on_autosync_toggled(bool checked);

    void on_maxTask_activated(int index);

private:
    Ui::ConfigPanel *ui;
};

#endif // CONFIGPANEL_H
