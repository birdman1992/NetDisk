#ifndef QCLIPBOARD_H
#define QCLIPBOARD_H

#include <QObject>
#include "QFolder/qfolder.h"

class QClipboard : public QObject
{
    Q_OBJECT
public:
    explicit QClipboard(QObject *parent = 0);

signals:

public slots:
};

#endif // QCLIPBOARD_H
