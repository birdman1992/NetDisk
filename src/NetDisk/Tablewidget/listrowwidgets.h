#ifndef LISTROWWIDGETS_H
#define LISTROWWIDGETS_H
#include <QCheckBox>
#include <QTableWidgetItem>
#include <QFolder/qfolder.h>
#include <QHBoxLayout>
#include <qobject.h>
#include <QWidget>
#include <QString>

class ListRowWidgets : public QWidget
{
    Q_OBJECT
public:
    ListRowWidgets(fileInfo* info,int _index);
    void setSelect(bool select);
    void clearSelectState();
    ~ListRowWidgets();
    QWidget* slctBox;
    QHBoxLayout* layout;
    QCheckBox* box;
    int index;
    QTableWidgetItem* file;
    QTableWidgetItem* fileSize;
    QTableWidgetItem* Modifytime;
    QTableWidgetItem* shareName;
    fileInfo* fInfo;
signals:
    void selectState(bool, int);

private slots:
    void selectChanged(bool);

private:
    void setFileIcon(QString fileName, QString fExt);
};

#endif // LISTROWWIDGETS_H
