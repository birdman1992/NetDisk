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
    TaskRow(){
        QHBoxLayout *hLayout = new QHBoxLayout;
        progress = new QProgressBar;
        proCell = new QWidget();
        hLayout->addWidget(progress);
        proCell->setLayout(hLayout);
        progress->setStyleSheet("QProgressBar {border: 0px solid grey;text-align: center;background-color: rgb(225, 230, 240);}\
                                          QProgressBar::chunk {background-color: rgb(194, 200, 204);width: 20px;}");
        progress->setMaximumHeight(5);
        progress->setTextVisible(false);
        progress->setRange(0,100);
    }
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
    QList<NetDonwloader*> list_downloader;//下载器队列
    NetDonwloader* downloadInfo;
    int downloadNum;
    int transNum;
    int progress;
    int colIndex_speed;
    int colIndex_state;
    int colIndex_progress;

    QString sizeofbytes(quint64 fsize);
    void listUiInit();
    void checkDownload();
    int getDownloadNum();

private slots:
    void progressCheck();
    void downloadFinish(DownloadInfo* info);

public slots:
    void newTask(netTrans*);
    void newDownloadTask(netTrans*);
};

#endif // TRANSLIST_H
