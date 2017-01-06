#include "qfolder.h"
#include "ui_qfolder.h"
#include <QCursor>

QFolder::QFolder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFolder)
{
    ui->setupUi(this);
    name.setGeometry(0,120,150,20);
    name.setFocusPolicy(Qt::NoFocus);
    name.setCursor(QCursor(Qt::ArrowCursor));
    this->setWindowFlags(Qt::FramelessWindowHint);

}

QFolder::~QFolder()
{
    delete ui;
}

void QFolder::focusInEvent(QFocusEvent* e)
{
    if(e == NULL)
        return;

    name.setFocusPolicy(Qt::ClickFocus);
}

void QFolder::focusOutEvent(QFocusEvent* e)
{
    if(e == NULL)
        return;
    name.setFocusPolicy(Qt::NoFocus);
}


/*************************************
CLASS QFolderLine
**************************************/
QFolderLine::QFolderLine(QLineEdit *parent) :
    QLineEdit(parent)
{
    this->setStyleSheet("QLineEdit#name{border: 0px solid;background: rgba(255,255,255,0);}QLineEdit#name:focus {border: 2px solid rgb(200, 200, 200);background:rgb(255, 255, 255);}");

}

void QFolderLine::focusInEvent(QFocusEvent* e)
{
    if(e == NULL)
        return;

    this->setCursor(QCursor(Qt::IBeamCursor));
}

void QFolderLine::focusOutEvent(QFocusEvent* e)
{
    if(e == NULL)
        return;
    this->setCursor(QCursor(Qt::ArrowCursor));
}









