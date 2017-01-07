#include "filespanel.h"
#include "ui_filespanel.h"
#include "QFolder/qfolder.h"

FilesPanel::FilesPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilesPanel)
{
    ui->setupUi(this);
}

FilesPanel::~FilesPanel()
{
    delete ui;
}
