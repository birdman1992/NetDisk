#ifndef PATHVIEW_H
#define PATHVIEW_H

#include <QWidget>
#include <QButtonGroup>
#include <QComboBox>
#include <QMenu>
#include "QFolder/qfolder.h"
#include "Pathview/pathbox.h"


namespace Ui {
class PathView;
}

class PathView : public QWidget
{
    Q_OBJECT

public:
    explicit PathView(QWidget *parent = 0);
    ~PathView();
    void pathUpdate(QList<QFolder*> newPath);

public slots:
    void pathChange(QList<fileInfo*> newPath);

private:
    Ui::PathView *ui;
    QList<fileInfo*> pathList;
    QList<PathBox*> fileBoxList;
    QButtonGroup pathBtn;

    void styleInit();
    void pathClear();
    void pathViewPaint(QList<fileInfo*> fileListIn);
    void paintEvent(QPaintEvent*);

private slots:
    void pathClicked(int index);
    void boxClicked(QString file);

signals:
    void cdRequest(double id);
};

#endif // PATHVIEW_H
