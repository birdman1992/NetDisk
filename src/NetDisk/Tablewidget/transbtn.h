#ifndef TRANSBTN_H
#define TRANSBTN_H
#include <qwidget.h>
#include <QAbstractItemDelegate>
#include <QStandardItemModel>


class TransBtn:public QAbstractItemDelegate
{
public:
    TransBtn(QObject *parent = 0);
//    void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
//    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};

#endif // TRANSBTN_H
