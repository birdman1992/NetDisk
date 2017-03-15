#ifndef LISTROWWIDGETS_H
#define LISTROWWIDGETS_H
#include <QCheckBox>
#include <QTableWidgetItem>
#include <QFolder/qfolder.h>
#include <QHBoxLayout>
#include <QWidget>
#include <QString>

class ListRowWidgets
{
public:
    ListRowWidgets(fileInfo* info);
    ~ListRowWidgets();
    QWidget* slctBox;
    QHBoxLayout* layout;
    QCheckBox* box;
    QTableWidgetItem* file;
    QTableWidgetItem* fileSize;
    QTableWidgetItem* Modifytime;
    QTableWidgetItem* shareName;
    fileInfo* fInfo;

private:
    void setFileIcon(QString fileName, QString fExt);
};

#endif // LISTROWWIDGETS_H
