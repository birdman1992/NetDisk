#ifndef QFOLDER_H
#define QFOLDER_H

#include <QWidget>
#include <qlineedit.h>

namespace Ui {
class QFolder;
}

class QFolderLine:public QLineEdit
{
    Q_OBJECT

public:
    explicit QFolderLine(QLineEdit *parent = 0);
    ~QFolderLine();

private:
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);
};

class QFolder : public QWidget
{
    Q_OBJECT

public:
    explicit QFolder(QWidget *parent = 0);
    ~QFolder();

private:
    Ui::QFolder *ui;
    QFolderLine name;
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);
};



#endif // QFOLDER_H
