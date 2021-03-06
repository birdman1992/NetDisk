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
    void pathChange(QList<QFileInfo *> newPath);

private:
    Ui::PathView *ui;
    QList<fileInfo*> pathList;
    QList<QFileInfo*> pathinfoList;
    QList<PathBox*> fileBoxList;
    QButtonGroup pathBtn;
    bool showLocal;

    void styleInit();
    void pathClear();
    void pathViewPaint(QList<fileInfo*> fileListIn);
    void pathViewPaint(QList<QFileInfo*> fileListIn);
    void paintEvent(QPaintEvent*);

private slots:
    void pathClicked(int index);
    void boxClicked(QString file);

signals:
    void cdRequest(double id);
    void cdRequest(int id);
};

#endif // PATHVIEW_H
