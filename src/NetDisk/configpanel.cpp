#include "configpanel.h"
#include "ui_configpanel.h"

ConfigPanel::ConfigPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigPanel)
{
    ui->setupUi(this);
}

ConfigPanel::~ConfigPanel()
{
    delete ui;
}
