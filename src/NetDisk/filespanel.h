#ifndef FILESPANEL_H
#define FILESPANEL_H

#include <QWidget>
#include <QList>
#include <QMenu>
#include <QAction>
#include <QCheckBox>
#include <QTableView>
#include <QTableWidget>
#include <QStandardItemModel>
#include <QTableWidgetItem>
#include "QFolder/qfolder.h"
#include "Http/nethttp.h"
#include "Tablewidget/selectdelegate.h"
#include "Tablewidget/listrowwidgets.h"
#include "Netsync/netsync.h"

namespace Ui {
class FilesPanel;
}

class FilesPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FilesPanel(QWidget *parent = 0);
    ~FilesPanel();
//    DiskFtp ftpClient;
    NetHttp* httpClient;
    NetSync* diskSync;
    int pageSize;//当前页大小
    int pageNum;//当前页码
    void panelShow(QList<QFolder*> fPanel);
    void panelClear();
    void panelRefresh();
    void panelCopy(QFolder* p);
    void panelPaste();
    void panelCd(fileInfo* dir);
    void panelCd(double dirId);
    void panelCdPage(int page);
    void panelBack();//返回
    void panelAhead();//前进
    void panelSearch(int searchType, QString name=QString());
    void panelShare();
    void fileDownload(fileInfo info);
    bool repeatCheck(QString* fName, QFolder* pFolder);
    void setViewMode(bool showList);
    double getCurId();
    void review();

public slots:
    void cmdCd(double id);
    void showDelete(bool show);
    void fileNew();
    void fileRefresh();
    void fileUpload();
    void fileSort();

private:
    Ui::FilesPanel *ui;
    QStandardItemModel* model;
    QList<QFolder*> curPanel;//当前面板包含的文件夹
    QList<QFolder*> dirTree;//目录树
    QList<fileInfo*> folderPath;//根目录到当前目录文件夹路径
    QList<ListRowWidgets*> checkList;
    QList<int> pathId;//根目录到当前目录文件夹ID
    int curIndex;//当前目录
    QFolder* pFolder;//文件夹指针
    QFolder* pClipboard;//剪贴板指针
    QString* pCdFolder;//cd指令目录指针
    bool pasteEnable;
    bool resizeEventEnable;
    bool showListView;//true:显示列表视图    false:显示平铺视图
    bool isResize;
    int showDeleteFolder;//显示删除文件
    double curDirId;//当前目录ID

    /***右键菜单***/
    QMenu* menu;
    QAction* act_new;
    QAction* act_upload;
    QAction* act_sort;
    QAction* act_refresh;
    QAction* act_paste;

    /***排序子菜单***/
    QMenu* menu_sort;
    QAction* act_filename;
    QAction* act_filesize;
    QAction* act_filetime;

    void contextMenuEvent(QContextMenuEvent*);
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent* size);
    void pathClear();
    short folderTypeJudge(QString fName,bool isDir);
    void showList(QList<QFolder*> fPanel);
    void checkListClear();

private slots:
    void httpGetListInfo(QList<fileInfo*>);
    void listViewCd(QModelIndex);

signals:
    void pathChanged(QList<fileInfo*>);//当前目录路径，当前目录的文件列表
    void historyEnable(bool backEnable, bool aheadEnable);//使能前进、后退箭头信号
    void newTask(netTrans*);
    void isLoading(bool);
    void scrollValueChanged(int value);
};

#endif // FILESPANEL_H
