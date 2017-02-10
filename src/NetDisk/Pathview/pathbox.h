#ifndef PATHBOX_H
#define PATHBOX_H

#include <QObject>
#include <QComboBox>
#include <QList>
#include <QString>

class PathBox : public QComboBox
{
    Q_OBJECT
public:
    explicit PathBox(QWidget *parent = 0);
    void setBoxPath(QList<QString*> list);
    void test();
private:
    QString fileDir;
private slots:
    void pathTrans(const QString &text);

signals:
    void pathSelected(const QString &text);
};

#endif // PATHBOX_H
