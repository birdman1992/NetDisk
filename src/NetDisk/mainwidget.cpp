#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QPushButton>
#include <QLineEdit>
#include <qscrollarea.h>
#include "filespanel.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    FilesPanel* diskPanel = new FilesPanel(this);
    QScrollArea* scrollFolder = new QScrollArea(this);
    scrollFolder->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollFolder->setWidgetResizable(true);
    ui->panelLayout->addWidget(scrollFolder);
    scrollFolder->setWidget(diskPanel);
}

MainWidget::~MainWidget()
{
    delete ui;
}

