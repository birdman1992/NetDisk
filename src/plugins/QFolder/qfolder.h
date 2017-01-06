#ifndef QFOLDER_H
#define QFOLDER_H

#include <QWidget>
#include <qlineedit.h>
#include <QAction>

namespace Ui {
class QFolder;
}

class QFolder : public QWidget
{
    Q_OBJECT

public:
    explicit QFolder(QWidget *parent = 0);
    ~QFolder();

private:
    Ui::QFolder *ui;

    /***右键菜单***/
    QAction* act_open;
    QAction* act_copy;
    QAction* act_cut;
    QAction* act_paste;
    QAction* act_delete;
    QAction* act_download;
    QAction* act_rename;


    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);
    void contextMenuEvent(QContextMenuEvent *);

signals:
    void nameFocus();

private slots:
    void folderRename();
    void nameFocused();

public slots:
    bool eventFilter(QObject *,QEvent *);
};



#endif // QFOLDER_H
