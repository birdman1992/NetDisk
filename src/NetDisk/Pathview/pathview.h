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
    void pathChange(QList<QString*> path, QList<QFolder*> fileList);

private:
    Ui::PathView *ui;
    QList<QString*> pathList;
    QList<PathBox*> fileBoxList;
    QButtonGroup pathBtn;

    void styleInit();
    void pathClear();
    void pathViewPaint(QList<QString*> pathIn, QList<QFolder*> fileListIn);
    void paintEvent(QPaintEvent*);

private slots:
    void pathClicked(int index);
    void boxClicked(QString file);

signals:
    void cdRequest(QString dir);
};

#endif // PATHVIEW_H
