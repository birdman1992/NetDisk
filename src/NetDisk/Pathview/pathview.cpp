#include "pathview.h"
#include "ui_pathview.h"
#include <QPushButton>
#include <QPainter>
#include <QMenuBar>
#include <QStandardItem>
#include <QtDebug>
#include <QCursor>


#define BTN_INTERVAL 5 //字体与按钮间隔


PathView::PathView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PathView)
{
    ui->setupUi(this);
    connect(&pathBtn, SIGNAL(buttonClicked(int)), this, SLOT(pathClicked(int)));
    styleInit();
}

PathView::~PathView()
{
    delete ui;
}

void PathView::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PathView::styleInit()
{
    this->setStyleSheet("\
                QPushButton{\
                    border-style: flat;\
                }\
                QPushButton:hover{\
                    color: rgb(0, 170, 255);\
                }\
                #PathView{\
                    border:1px solid rgb(187, 187, 187);\
                    background-color: rgb(255, 255, 255);\
                }\
                QComboBox::drop-down {\
                    subcontrol-origin: padding;\
                    subcontrol-position: top right;\
                    width: 15px;\
                    border-top-right-radius: 3px;\
                }\
                QComboBox{color: rgb(0, 170, 255);\
                    border: 5px; image: url(:/imgs/arrow_rignt.png);\
                }\
                QComboBox:on{\
                    border: 5px; image: url(:/imgs/arrow_down.png);}"
                           );
}

void PathView::pathClear()
{
    QList<QAbstractButton*> btns = pathBtn.buttons();
    QPushButton* btn;
    PathBox* pbox;

    for(; ; )
    {
        if(btns.isEmpty())
            break;
        btn = (QPushButton*)btns.takeFirst();
        pathBtn.removeButton(btn);
        btn->setHidden(true);
        btn->deleteLater();
    }

    for(; ; )
    {
        if(fileBoxList.isEmpty())
            break;
        pbox = fileBoxList.takeFirst();
        disconnect(pbox, SIGNAL(pathSelected(QString)), this, SLOT(boxClicked(QString)));
        pbox->setHidden(true);
        pbox->deleteLater();
    }
}

/***更新路径栏接口***/
void PathView::pathUpdate(QList<QFolder *> newPath)
{
    QString* str;
    int i = 0;

    if(newPath.isEmpty())
        return;

    for(i=0; i<newPath.count(); i++)
    {
        str = new QString(newPath.at(i)->fileName());
        pathList<<str;
    }

//    pathViewPaint(pathList);
}


/*****************
公有槽函数
******************/

/***更新路径栏槽函数***/
void PathView::pathChange(QList<QString*> path, QList<QFolder*> fileList)
{qDebug("pathchange");
    QString* str = new QString("我的网盘");
    pathList = path;
    pathList.insert(0,str);
    pathViewPaint(pathList, fileList);
}

/*****************
私有槽函数
******************/
void PathView::pathClicked(int index)
{
    if(pathList.count() <= index)
        return;

    QString str = "/";

    for(int i=1; i<index; i++)
    {
        str += *(pathList.at(i)) + "/";
    }
    qDebug()<<"pathClicked"<<str;
    emit cdRequest(str);
}

void PathView::boxClicked(QString file)
{
    qDebug()<<"boxClicked"<<file;
    emit cdRequest(file);
}



void PathView::pathViewPaint(QList<QString*> pathIn, QList<QFolder*> fileListIn)
{
    int x_offset = 0;
    int _size = this->geometry().height()-2;
    int i;
    int j;
    QString* str;
    QPushButton *pBtn;
    PathBox *pBox1;
    QFont wordFont;
    QRect rct;

    wordFont.setFamily("Courier");
    wordFont.setPixelSize(this->geometry().height()/2);
    QFontMetrics fm(wordFont);

    pathClear();

    for(i=0; i<pathIn.count(); i++)
    {
        str = pathIn.at(i);//qDebug()<<"dir"<<*str;
        pBtn = new QPushButton(*str,this);
        pBtn->setFont(wordFont);
        pathBtn.addButton(pBtn,i);
        rct = fm.boundingRect(*str);
        pBtn->resize(rct.width()+BTN_INTERVAL,_size);
        pBtn->move(x_offset, 1);
        x_offset += pBtn->width();
        pBtn->setCursor(QCursor(Qt::PointingHandCursor));
        pBtn->show();

        pBox1 = new PathBox(this);
        pBox1->resize(_size/2, _size);

        pBox1->move(x_offset, 1);

        x_offset += pBox1->width();

        for(j=0; j<fileListIn.count(); j++)
        {qDebug()<<"folder"<<fileListIn.at(j)->fileName();
            pBox1->insertItem(j,QIcon(":/imgs/folder_m.ico"),QString(fileListIn.at(j)->fileName()));
        }
        connect(pBox1, SIGNAL(pathSelected(QString)), this, SLOT(boxClicked(QString)));
        pBox1->setBoxPath(pathIn);
        pBox1->setCurrentIndex(-1);
        pBox1->view()->setFixedWidth(100);
        pBox1->setCursor(QCursor(Qt::PointingHandCursor));
        fileBoxList<<pBox1;//qDebug()<<"boxlist size"<<fileBoxList.count();
        pBox1->show();
//        if(fileBoxList.count()>2)
//            pathClear();
    }
}
