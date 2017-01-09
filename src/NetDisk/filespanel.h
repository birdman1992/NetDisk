#ifndef FILESPANEL_H
#define FILESPANEL_H

#include <QWidget>
#include <QList>
#include "QFolder/qfolder.h"

namespace Ui {
class FilesPanel;
}

class FilesPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FilesPanel(QWidget *parent = 0);
    ~FilesPanel();
    void panelShow(QList<QFolder*> fPanel);

private:
    Ui::FilesPanel *ui;
    QList<QFolder*> curPanel;
};

#endif // FILESPANEL_H
