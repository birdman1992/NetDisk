#ifndef FILESPANEL_H
#define FILESPANEL_H

#include <QWidget>
#include <QList>
#include <QMenu>
#include <QAction>
#include <QTableWidget>
#include "QFolder/qfolder.h"
#include "Ftp/diskftp.h"
#include "Http/nethttp.h"

namespace Ui {
class FilesPanel;
}

class FilesPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FilesPanel(QWidget *parent = 0);
    ~FilesPanel();
    DiskFtp ftpClient;
    NetHttp* httpClient;
    void panelShow(QList<QFolder*> fPanel);
    void panelClear();
    void panelRefresh();
    void panelCopy(QFolder* p);
    void panelPaste();
    void panelCd(int dirId);
    bool repeatCheck(QString* fName, QFolder* pFolder);
    void addFolder(QFolder* parFolder);
    void setViewMode(bool showList);
    QString getCurPath();

public slots:
    void cmdCd(QString dir);

private:
    Ui::FilesPanel *ui;
    QList<QFolder*> curPanel;//当前面板包含的文件夹
    QList<QFolder*> dirTree;//目录树
    QList<QString*> folderPath;//根目录到当前目录文件夹路径
    QList<int> pathId;//根目录到当前目录文件夹ID
    QFolder* curDir;//当前目录
    QFolder* pFolder;//文件夹指针
    QFolder* pClipboard;//剪贴板指针
    QString* pCdFolder;//cd指令目录指针
    bool pasteEnable;
    bool showListView;//true:显示列表视图    false:显示平铺视图
    int curDirId;//当前目录ID
    int pageSize;//当前页大小
    int pageNum;//当前页码

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
    void pathClear();
    short folderTypeJudge(QString fName,bool isDir);

private slots:
    void fileNew();
    void fileRefresh();
    void fileUpload();
    void fileSort();

    void httpGetListInfo(QList<fileInfo*>);
    void ftpListShow();
    void ftpCdFinishi();

signals:
    void pathChanged(QList<QString*>, QList<QFolder*>);//当前目录路径，当前目录的文件列表
};

#endif // FILESPANEL_H
