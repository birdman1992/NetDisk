#include "configpanel.h"
#include "ui_configpanel.h"
#include "netconfig.h"
#include <qfiledialog.h>
#include <QtDebug>
#include <QSize>

ConfigPanel::ConfigPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigPanel)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->downloadPath->setText(netConf->getDownloadPath());
    ui->syncPath->setText(netConf->getSyncPath());
    ui->autosync->setChecked(netConf->autoSyncDir());
    ui->serverAddr->setText(netConf->getServerAddress());
    ui->maxTask->addItem("1");
    ui->maxTask->addItem("2");
    ui->maxTask->addItem("3");
    ui->maxTask->addItem("4");
    ui->maxTask->addItem("5");
    ui->maxTask->setCurrentIndex(netConf->getMaxTaskNum()-1);
    ui->warn_address->hide();
    initStack();
    configFinshed = configCheck();
}

ConfigPanel::~ConfigPanel()
{
    delete ui;
}

void ConfigPanel::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool ConfigPanel::configCheck()
{
    if(netConf->getDownloadPath().isEmpty())
    {
        ui->warn_download->show();
        return false;
    }
    else
        ui->warn_download->hide();

    if(netConf->getSyncPath().isEmpty())
    {
        ui->warn_sync->show();
        return false;
    }
    else
        ui->warn_sync->hide();

    QString str = ui->serverAddr->text();
    if(str.isEmpty())
    {
        ui->warn_address->setText("请配置服务器地址");
        ui->warn_address->show();
        ui->listWidget->setCurrentRow(1);
        return false;
    }
    QString pattern("http://*.*.*.*:*");
    QRegExp rx(pattern);
    rx.setPatternSyntax(QRegExp::Wildcard);
    if(rx.exactMatch(str))
    {
        ui->warn_address->hide();
        netConf->setServerAddress(str);
        netConf->saveAll();
        return true;
    }
    else
    {
        ui->warn_address->setText("地址格式错误");
        ui->warn_address->show();
        ui->listWidget->setCurrentRow(1);
    }
    return false;
}

bool ConfigPanel::configIsFinished()
{
    return configFinshed;
}

void ConfigPanel::on_downloadSet_clicked()
{
    QString newDir = QFileDialog::getExistingDirectory(this,"选择下载目录",netConf->getDownloadPath(),QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(!newDir.isEmpty())
    {
        ui->downloadPath->setText(newDir);
        netConf->setDownloadPath(newDir);
        netConf->saveAll();
    }
    configCheck();
    return;
}

void ConfigPanel::on_synccSet_clicked()
{
    QString newDir = QFileDialog::getExistingDirectory(this,"选择下载目录",netConf->getDownloadPath(),QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(!newDir.isEmpty())
    {
        ui->syncPath->setText(newDir);
        netConf->setSyncPath(newDir);
        netConf->saveAll();
    }
    configCheck();
    return;
}

void ConfigPanel::on_autosync_toggled(bool checked)
{
    netConf->setautoSync(checked);
    netConf->saveAll();
}

void ConfigPanel::on_maxTask_activated(int index)
{
    netConf->setMaxTaskNum(index+1);
    netConf->saveAll();
}

void ConfigPanel::on_finish_clicked()
{
    if(configCheck())
    {
        configFinshed = true;
        emit configOver();
        this->close();
        return;
    }
}

void ConfigPanel::on_serverAddr_editingFinished()
{
    configCheck();
}

void ConfigPanel::initStack()
{
    QListWidgetItem* item;
    item = new QListWidgetItem("基本");
    item->setSizeHint(QSize(138,38));
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->listWidget->addItem(item);

    item = new QListWidgetItem("传输");
    item->setSizeHint(QSize(138,38));
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->listWidget->addItem(item);
    ui->listWidget->setCurrentRow(0);
    ui->stackedPanel->setCurrentIndex(0);
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedPanel, SLOT(setCurrentIndex(int)));
}

void ConfigPanel::on_cancel_clicked()
{
    this->close();
}
