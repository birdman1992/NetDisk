#ifndef QFOLDER_H
#define QFOLDER_H

#include <QWidget>
#include <qlineedit.h>
#include <QAction>
#include <QMouseEvent>
#include <QString>
#include <QList>
#include <QDateTime>
#include "Http/nethttp.h"

namespace Ui {
class QFolder;
}

class QFolder : public QWidget
{
    Q_OBJECT

public:
    explicit QFolder(QWidget *parent = 0,fileInfo* info = NULL);
    ~QFolder();
    void setPasteEnable(bool enable);
    void rename();
    void newfolder();
    void setParFolder(QFolder* par);
    void setFolderTime(QDateTime fTime);
    short folderType();
    QString fileName();

    /***属性***/
    enum folderType
    {
        DIR_COMMON,
        DIR_SHARE,
        DIR_ENSHRINE,
        DIR_LOCK,
        FILE_APK,
        FILE_DOC,
        FILE_MP3,
        FILE_MP4,
        FILE_PDF,
        FILE_PPT,
        FILE_RAR,
        FILE_TXT,
        FILE_XLS,
        FILE_ZIP,
        FILE_DEFAULT,
    };

private:
    Ui::QFolder *ui;
    /***目录结构***/
    QList<QFolder*> subdir;
    QFolder* pardir;

    /***右键菜单***/
    QMenu* menu;
    QAction* act_open;
    QAction* act_copy;
    QAction* act_cut;
    QAction* act_paste;
    QAction* act_delete;
    QAction* act_download;
    QAction* act_rename;

    QDateTime folderTime;
    short fType;
    bool pasteEnable;
    bool selectEnable;
    bool newfile;
    QString folderName;
    fileInfo folderInfo;
    int textwid;

    void paintEvent(QPaintEvent*);
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);
    void contextMenuEvent(QContextMenuEvent *);
    void mouseDoubleClickEvent(QMouseEvent*);
    bool event(QEvent *event);

    QString folderNameCut(QString strIn, int cutLength);
    void setImg();
    short folderTypeJudge(QString fName);

protected:
    ushort t;

signals:
    void nameFocus();
    void copy(QString srcPath);

private slots:
    void nameFocused();
    void editFinish();

    //菜单
    void folderRename();
    void folderOpen();
    void folderCopy();
    void folderCut();
    void folderPaste();
    void folderDelete();
    void folderDownload();

public slots:
    bool eventFilter(QObject *,QEvent *);
};



#endif // QFOLDER_H
