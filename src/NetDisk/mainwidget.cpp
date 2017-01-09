#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QPushButton>
#include <QLineEdit>
#include "filespanel.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    FilesPanel* diskPanel = new FilesPanel(this);
    ui->panelLayout->addWidget(diskPanel);
}

MainWidget::~MainWidget()
{
    delete ui;
}

