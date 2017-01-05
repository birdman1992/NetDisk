#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QPushButton>
#include <QLineEdit>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    connect((ui->line), &QLineEdit::editingFinished, ui->line,&QLineEdit::clearFocus);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::on_btn_clicked()
{
    ui->line->setFocus();
    ui->line->selectAll();
}
