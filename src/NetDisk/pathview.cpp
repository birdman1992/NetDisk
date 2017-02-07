#include "pathview.h"
#include "ui_pathview.h"

PathView::PathView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PathView)
{
    ui->setupUi(this);
}

PathView::~PathView()
{
    delete ui;
}
