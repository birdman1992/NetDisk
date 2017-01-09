#include "filespanel.h"
#include "ui_filespanel.h"
#include "QFolder/qfolder.h"

#define FOLDER_SIZE 100

FilesPanel::FilesPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilesPanel)
{
    ui->setupUi(this);
    QFolder *fode = new QFolder(this);
    fode->show();;
}

FilesPanel::~FilesPanel()
{
    delete ui;
}

void FilesPanel::panelShow(QList<QFolder*> fPanel)
{
    int i = 0;

    for(i=0; i<fPanel.count(); i++)
    {

    }
}
