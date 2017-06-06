#ifndef TRANSLIST_H
#define TRANSLIST_H

#include <QWidget>
#include <QTimer>
#include <QList>
#include <QProgressBar>
#include <QHBoxLayout>
#include "Translist/bardelegata.h"
#include "Http/nettrans.h"
#include "Http/netdonwloader.h"
namespace Ui {
class TransList;
}
class netTrans;

class TaskRow
{
public:
    TaskRow();
    ~TaskRow();
    QWidget* proCell;
    QProgressBar* progress;
    netTrans* trans;
};

class TransList : public QWidget
{
    Q_OBJECT

public:
    explicit TransList(QWidget *parent = 0);
    int download(DownloadInfo* info);

    ~TransList();

private:
    Ui::TransList *ui;
    QTimer tProgress;
    TransModel* transModel;
    QList<netTrans*> taskList;
    QList<TaskRow*> taskDownload;
    QList<TaskRow*> taskUpload;
    QList<NetDonwloader*> list_downloader;//下载器队列
    NetDonwloader* downloadInfo;
    int downloadNum;
    int transNum;
    int progress;
    int colIndex_speed;
    int colIndex_state;
    int colIndex_progress;
    int listState;

    QString sizeofbytes(quint64 fsize);
    void listUiInit();
    void checkDownload();
    int getDownloadNum();

    int getUploadNum();
    void checkUpload();

signals:
    void taskClear();

private slots:
    void progressCheck();
    void downloadFinish(DownloadInfo* info);

public slots:
    void newTask(netTrans*);
    void newDownloadTask(netTrans*);
    void newUploadTask(netTrans*);
    void setValue(int);
};

#endif // TRANSLIST_H
