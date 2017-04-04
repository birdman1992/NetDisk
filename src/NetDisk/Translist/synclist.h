#ifndef SYNCLIST_H
#define SYNCLIST_H

#include <QWidget>
#include <Http/nethttp.h>
#include <QFileInfo>
#include <QDir>
#include <QList>
#include <QFileInfoList>
#include <QStyledItemDelegate>
#include <QShowEvent>

namespace Ui {
class syncList;
}

class syncList : public QWidget
{
    Q_OBJECT

public:
    explicit syncList(QWidget *parent = 0);
    ~syncList();
    void initTable(syncTable *table);
    void goAhead();
    void goBack();

public slots:
    void cmdCd(int);
    void syncUploadMsg(int);
    void syncDownloadMsg(int);

private slots:
    void on_tableWidget_doubleClicked(const QModelIndex &index);

private:
    Ui::syncList *ui;
    syncTable* checkTable;
    QString currentDir;
    int currentIndex;
    int downloadNum;
    int uploadNum;

    QFileInfoList list_show;
    QList<QFileInfo*> list_path;
    void showEvent(QShowEvent* event);

    void showList();

    QString getFolderType(QFileInfo fInfo);
    QString getFolderSize(quint64 fileSize);
    void updateSyncMessage();
    void syncRefresh();

signals:
    void pathChanged(QList<QFileInfo*>);
    void historyEnable(bool,bool);
    void syncNumChanged(int upNum, int downNum);
};

class NoFocusDelegate:public QStyledItemDelegate
{
public:
    NoFocusDelegate(QStyledItemDelegate* parent=0);

private:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // SYNCLIST_H
