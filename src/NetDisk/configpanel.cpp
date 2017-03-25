#include "configpanel.h"
#include "ui_configpanel.h"
#include "netconfig.h"
#include <qfiledialog.h>
#include <QtDebug>

ConfigPanel::ConfigPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigPanel)
{
    ui->setupUi(this);
    ui->downloadPath->setText(netConf->getDownloadPath());
    ui->syncPath->setText(netConf->getSyncPath());
    ui->autosync->setChecked(netConf->autoSyncDir());
    ui->maxTask->addItem("1");
    ui->maxTask->addItem("2");
    ui->maxTask->addItem("3");
    ui->maxTask->addItem("4");
    ui->maxTask->addItem("5");
    ui->maxTask->setCurrentIndex(netConf->getMaxTaskNum()-1);
}

ConfigPanel::~ConfigPanel()
{
    delete ui;
}

void ConfigPanel::on_downloadSet_clicked()
{
    QString newDir = QFileDialog::getExistingDirectory(this,"选择下载目录",netConf->getDownloadPath(),QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(newDir.isEmpty())
        return;
    else
    {
        ui->downloadPath->setText(newDir);
        netConf->setDownloadPath(newDir);
    }
    netConf->saveAll();
}

void ConfigPanel::on_synccSet_clicked()
{
    QString newDir = QFileDialog::getExistingDirectory(this,"选择下载目录",netConf->getDownloadPath(),QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(newDir.isEmpty())
        return;
    else
    {
        ui->syncPath->setText(newDir);
        netConf->setSyncPath(newDir);
    }
    netConf->saveAll();
}

void ConfigPanel::on_autosync_toggled(bool checked)
{
    netConf->setautoSync(checked);
    netConf->saveAll();
}

void ConfigPanel::on_maxTask_activated(int index)
{

}
