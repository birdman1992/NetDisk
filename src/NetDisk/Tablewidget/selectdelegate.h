#ifndef SELECTDELEGATE_H
#define SELECTDELEGATE_H
#include <QAbstractItemDelegate>

class SelectDelegate : public QAbstractItemDelegate
{
public:
    SelectDelegate( QObject *parent = 0 );
      // 覆盖两个函数就可以显示进度条
      void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
      QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;
};

#endif // SELECTDELEGATE_H
