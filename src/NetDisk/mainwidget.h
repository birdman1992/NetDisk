#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPoint>
#include <QListWidgetItem>
#include <qscrollarea.h>
#include <QresizeEvent>
#include <Translist/translist.h>
#include "Pathview/pathview.h"
#include "filespanel.h"
#include "loading.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

signals:
    void functionNew();
    void functionUpload();
    void functionDonwload();
    void functionDelete();

private slots:
    void on_wMin_clicked();
    void on_wClose_clicked();
    void on_wMax_toggled(bool checked);
    void founctionListClicked(QListWidgetItem* item);
    void functionBtnClicked(QModelIndex index);
    void on_back_clicked();
    void on_forward_clicked();
    void on_refresh_clicked();
    void on_translist_toggled(bool checked);
    void searchTypeChanged(int);
    void on_searchBtn_clicked();
    void isLoading(bool);

public slots:
    void historyEnabled(bool backEnable, bool aheadEnable);

private:
    Ui::MainWidget *ui;
    loading loadingUi;
    QScrollArea* scrollFolder;
    FilesPanel* diskPanel;
    PathView* pathView;
    TransList* transList;
    bool wMoveable;
    QPoint dragPosition;
    int fType;

    void initSearch();
    void initSysMenu();
    void initSilidebar();
    void initFunctionList();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // MAINWIDGET_H
