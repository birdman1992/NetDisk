#ifndef LINKROWWIDGETS_H
#define LINKROWWIDGETS_H

#include <QWidget>

class LinkInfo
{
    int USER_ID;
    int CLICK_COUNT;
    QString PASS_TIME;
    QString ADD_TIME;
    int SAVE_COUNT;
    QString KEY;
    int STATUS;
    int FILE_STATUS;
    QString ISPASS;
    QString ICON;
    QString FILE_NAME;
    int DOWN_COUNT;
    QString LINK;
    double ID;
    int SERVER_ID;
    QString URL;
    QString PASSWORD;
    QString TYPE;
};

class LinkRowWidgets : public QWidget
{
    Q_OBJECT
public:
    explicit LinkRowWidgets(LinkInfo* info, QWidget *parent = 0);

signals:

public slots:
};

#endif // LINKROWWIDGETS_H
