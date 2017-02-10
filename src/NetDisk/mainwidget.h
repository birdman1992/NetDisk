#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPoint>
#include "Pathview/pathview.h"
#include "filespanel.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

private slots:

    void on_wMin_clicked();

    void on_wClose_clicked();

    void on_wMax_toggled(bool checked);

private:
    Ui::MainWidget *ui;
    FilesPanel* diskPanel;
    PathView* pathView;
    bool wMoveable;
    QPoint dragPosition;

    void initSilidebar();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // MAINWIDGET_H
