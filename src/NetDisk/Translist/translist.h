#ifndef TRANSLIST_H
#define TRANSLIST_H

#include <QWidget>
#include <QTimer>
#include <QList>
#include "Translist/bardelegata.h"
#include "Http/nettrans.h"
#include "Http/netdonwloader.h"
namespace Ui {
class TransList;
}
class netTrans;

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
    QList<NetDonwloader*> list_downloader;//下载器队列
    NetDonwloader* downloadInfo;
    int transNum;
    int progress;
    int colIndex_speed;
    int colIndex_state;
    int colIndex_progress;

    QString sizeofbytes(quint64 fsize);
    void listUiInit();

private slots:
    void progressCheck();
    void downloadFinish(DownloadInfo* info);

public slots:
    void newTask(netTrans*);
};

#endif // TRANSLIST_H
