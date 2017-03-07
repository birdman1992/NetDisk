#include <qpainter.h>
#include <QItemDelegate>
#include <QProgressBar>
#include <QApplication>
#include <QtDebug>
#include <qstyleoption.h>
#include <qprogressbar.h>
#include <QStyledItemDelegate>
#include "bardelegata.h"

BarDelegate::BarDelegate( QObject *parent ) : QAbstractItemDelegate( parent ) { }

void BarDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    if (index.column() == 3)
    {
        int nProgress = index.model()->data(index, Qt::DisplayRole).toInt();
        qDebug()<<nProgress;
        // 设置进度条的风格
        QStyleOptionProgressBar progressBarOption;
        progressBarOption.initFrom(option.widget);
        // 设置进度条显示的区域
        progressBarOption.rect = option.rect.adjusted(4, 4, -4, -4);//QRect(option.rect.left() + nLeft, option.rect.top() + nTop,  nWidth, nHeight);
        // 设置最小值
        progressBarOption.minimum = 0;
        // 设置最大值
        progressBarOption.maximum = 100;
        // 设置对齐方式
        progressBarOption.textAlignment = Qt::AlignCenter;
        // 设置进度
        progressBarOption.progress = nProgress;
        // 设置文本（百分比）
        progressBarOption.text = QString("%1%").arg(nProgress);
        // 设置文本可见
        progressBarOption.textVisible = true;

        QProgressBar progressBar;

        //绘制进度条
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter, &progressBar);
    }
}

QSize BarDelegate::sizeHint( const QStyleOptionViewItem &, const QModelIndex & ) const
{
    return QSize( 45, 15 ); // 随便改，不影响（我认为）
}
