#ifndef QFOLDER_H
#define QFOLDER_H

#include <QWidget>
#include <qlineedit.h>
#include <QAction>
#include <QMouseEvent>
#include <QString>
#include <QList>

namespace Ui {
class QFolder;
}

class QFolder : public QWidget
{
    Q_OBJECT

public:
    explicit QFolder(QWidget *parent = 0,short _type = 2,QString fName = "新建文件夹");
    ~QFolder();
    void setPasteEnable(bool enable);

private:
    Ui::QFolder *ui;
    /***目录结构***/
    QList<QFolder*> subdir;

    /***右键菜单***/
    QAction* act_open;
    QAction* act_copy;
    QAction* act_cut;
    QAction* act_paste;
    QAction* act_delete;
    QAction* act_download;
    QAction* act_rename;

    /***属性***/
    enum folderType
    {
        DIR,
        FILE,
    };
    short fType;
    bool pasteEnable;
    bool selectEnable;
    QString folderName;

    void paintEvent(QPaintEvent*);
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);
    void contextMenuEvent(QContextMenuEvent *);
    void mouseDoubleClickEvent(QMouseEvent*);

protected:
    ushort t;

signals:
    void nameFocus();

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
