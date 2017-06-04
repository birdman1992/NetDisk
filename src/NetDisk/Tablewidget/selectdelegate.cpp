#include "selectdelegate.h"
#include <QCheckBox>
#include <QApplication>
#include <QStyledItemDelegate>

SelectDelegate::SelectDelegate( QObject *parent ) : QAbstractItemDelegate( parent ) { }

void SelectDelegate::paint( QPainter *painter, const QStyleOptionViewItem&, const QModelIndex &index ) const
{
    //获取值
    bool checked = index.model()->data(index, Qt::DisplayRole).toBool();
    //按钮的风格选项
    QStyleOptionButton *checkBoxOption = new QStyleOptionButton();
    checkBoxOption->state |= QStyle::State_Enabled;
    //根据值判断是否选中
    if(checked)
    {
        checkBoxOption->state |= QStyle::State_On;
    }
    else
    {
        checkBoxOption->state |= QStyle::State_Off;
    }
    //返回QCheckBox几何形状
//    checkBoxOption->rect = CheckBoxRect(option);
    //绘制QCheckBox
    QApplication::style()->drawControl(QStyle::CE_CheckBox,checkBoxOption,painter);
}

QSize SelectDelegate::sizeHint( const QStyleOptionViewItem &, const QModelIndex & ) const
{
    return QSize( 45, 15 ); // 随便改，不影响（我认为）
}
