#include <qpainter.h>
#include <QItemDelegate>
#include <QProgressBar>
#include <QApplication>
#include <QtDebug>
#include "bardelegata.h"

BarDelegate::BarDelegate( QObject *parent ) : QAbstractItemDelegate( parent ) { }

void BarDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    if( option.state & QStyle::State_Selected )
      painter->fillRect( option.rect, option.palette.highlight() );

    // 数据是存储在QStandardItemModel的QStandardItem，会自动根据当前行进行匹配（我认为）
    int value = index.model()->data( index, Qt::DisplayRole ).toInt(); // 这句，取得当前行的数据
    qDebug() << value;
    double factor = double(value)/100.0; // 计算比例因子

    painter->save(); // 保存旧画板（我认为）
    // 进度条背景色
    if( factor > 0.8 )
    {
      painter->setBrush( Qt::red ); // 超过0.8画纯红色
      factor = 1;
    }
    else
      painter->setBrush( QColor( 0, int(factor*255), 255-int(factor*255) ) ); // 否则颜色依次变淡

    painter->setPen( Qt::black ); // 画笔颜色（这里没用到，我认为）
    // 前面都是准备工作，这里才是真正画进度条
    painter->drawRect( option.rect.x()+2, option.rect.y()+2, int(factor*(option.rect.width()-5)), option.rect.height()-5 );
    painter->restore(); // 恢复新画板（我认为）
}

QSize BarDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    return QSize( 45, 15 ); // 随便改，不影响（我认为）
}
