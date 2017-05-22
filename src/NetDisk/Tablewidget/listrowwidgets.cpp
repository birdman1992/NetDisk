#include "listrowwidgets.h"
#include <QIcon>

ListRowWidgets::ListRowWidgets(fileInfo* info, int _index)
{
    slctBox = new QWidget(this);
    layout = new QHBoxLayout(this);
    box = new QCheckBox(this);
    index = _index;
    file = NULL;
    fInfo = info;
    setFileIcon(info->FILE_NAME, info->EXT);
    layout->addWidget(box);
    layout->setMargin(0);
    layout->setAlignment(box, Qt::AlignCenter);
    slctBox->setLayout(layout);
    fileSize = new QTableWidgetItem(QString::number(info->SIZE));
    Modifytime = new QTableWidgetItem(info->LAST_MOD_TIME.toString("yyyy-MM-dd hh:mm:ss dddd"));
    shareName = new QTableWidgetItem(info->REAL_NAME);
    connect(box, SIGNAL(toggled(bool)), this, SLOT(selectChanged(bool)));
}

void ListRowWidgets::setSelect(bool select)
{
    box->setChecked(select);
}

void ListRowWidgets::clearSelectState()
{
    box->setChecked(false);
}

ListRowWidgets::~ListRowWidgets()
{
    delete box;
    slctBox->deleteLater();
    layout->deleteLater();
}

void ListRowWidgets::selectChanged(bool selected)
{
    emit selectState(selected, index);
}

void ListRowWidgets::setFileIcon(QString fileName, QString fExt)
{
    if(fExt.isEmpty())
    {
        if(fileName == "共享文件夹")
            file = new QTableWidgetItem(QIcon(":/imgs/32x32/共享文件夹.png"),fileName);
        else if(fileName == "私密文件")
            file = new QTableWidgetItem(QIcon(":/imgs/32x32/锁定文件夹.png"),fileName);
        else if(fileName == "我的收藏")
            file = new QTableWidgetItem(QIcon(":/imgs/32x32/收藏文件夹.png"),fileName);
        else
            file = new QTableWidgetItem(QIcon(":/imgs/32x32/普通文件夹.png"),fileName);
    }
    else
    {
        if(fExt.endsWith("apk", Qt::CaseInsensitive))
            file = new QTableWidgetItem(QIcon(":/imgs/32x32/apk.png"),fileName);
        else if(fExt.endsWith("doc", Qt::CaseInsensitive) || fExt.endsWith("docx"))
            file = new QTableWidgetItem(QIcon(":/imgs/32x32/doc.png"),fileName);
        else if(fExt.endsWith("mp3", Qt::CaseInsensitive))
            file = new QTableWidgetItem(QIcon(":/imgs/32x32/mp3.png"),fileName);
        else if(fExt.endsWith("mp4", Qt::CaseInsensitive))
            file = new QTableWidgetItem(QIcon(":/imgs/32x32/mp4.png"),fileName);
        else if(fExt.endsWith("pdf", Qt::CaseInsensitive))
            file = new QTableWidgetItem(QIcon(":/imgs/32x32/pdf.png"),fileName);
        else if(fExt.endsWith("ppt", Qt::CaseInsensitive))
            file = new QTableWidgetItem(QIcon(":/imgs/32x32/ppt.png"),fileName);
        else if(fExt.endsWith("rar", Qt::CaseInsensitive))
            file = new QTableWidgetItem(QIcon(":/imgs/32x32/rar.png"),fileName);
        else if(fExt.endsWith("txt", Qt::CaseInsensitive))
            file = new QTableWidgetItem(QIcon(":/imgs/32x32/txt.png"),fileName);
        else if(fExt.endsWith("xls", Qt::CaseInsensitive) || fExt.endsWith("xlsx", Qt::CaseInsensitive))
            file = new QTableWidgetItem(QIcon(":/imgs/32x32/xls.png"),fileName);
        else if(fExt.endsWith("zip", Qt::CaseInsensitive))
            file = new QTableWidgetItem(QIcon(":/imgs/32x32/zip.png"),fileName);
        else
            file = new QTableWidgetItem(QIcon(":/imgs/32x32/缺省文件格式.png"),fileName);
    }
}


