#ifndef FILESPANEL_H
#define FILESPANEL_H

#include <QWidget>
#include <QList>
#include <QMenu>
#include <QAction>
#include "QFolder/qfolder.h"
#include "Ftp/diskftp.h"

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
    void panelShow(QList<QFolder*> fPanel);
    void addFolder(QFolder* parFolder);
    QString getCurPath();

private:
    Ui::FilesPanel *ui;
    QList<QFolder*> curPanel;//当前面板包含文件夹
    QList<QFolder*> dirTree;//目录树
    QList<QFolder*> folderPath;//根目录到当前目录文件夹路径
    QFolder* curDir;//当前目录
    QFolder* pFolder;//文件夹指针
    bool pasteEnable;

    /***右键菜单***/
    QMenu* menu;
    QAction* act_new;
    QAction* act_upload;
    QAction* act_sort;
    QAction* act_refresh;
    QAction* act_paste;

    //排序子菜单
    QMenu* menu_sort;
    QAction* act_filename;
    QAction* act_filesize;
    QAction* act_filetime;

    void contextMenuEvent(QContextMenuEvent*);
    void paintEvent(QPaintEvent*);

private slots:
    void fileNew();
    void fileRefresh();
    void fileUpload();
    void fileSort();
    void ftpGetListInfo(QUrlInfo info);

};

#endif // FILESPANEL_H
