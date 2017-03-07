#ifndef BARDELEGATA_H
#define BARDELEGATA_H
#include <QAbstractItemDelegate>
#include <QStandardItemModel>


class BarDelegate : public QAbstractItemDelegate
{
public:
    BarDelegate( QObject *parent = 0 );
      // 覆盖两个函数就可以显示进度条
      void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
      QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;
};

//代理类，把所有单元格中的字符居中显示
class TransModel : public QStandardItemModel
{
    Q_OBJECT
public:
    TransModel(QObject *parent=NULL) : QStandardItemModel(parent) { }
    TransModel(int row, int column, QObject *parent=NULL)
        : QStandardItemModel(row, column, parent) { }
};

#endif // BARDELEGATA_H
