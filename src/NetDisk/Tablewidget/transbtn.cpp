#include "transbtn.h"

TransBtn::TransBtn( QObject *parent ) : QAbstractItemDelegate( parent ) { }

//void TransBtn::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{

//}

//bool TransBtn::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
//{
//    if (event->type() == QEvent::MouseButtonPress)
//    {

//        QMouseEvent* e =(QMouseEvent*)event;

//        if (m_btns.contains(index))
//        {
//            QPair<QStyleOptionButton*, QStyleOptionButton*>* btns = m_btns.value(index);
//            if (btns->first->rect.contains(e->x(), e->y()))
//            {
//                btns->first->state |= QStyle::State_Sunken;
//            }
//            else if(btns->second->rect.contains(e->x(), e->y()))
//            {
//                btns->second->state |= QStyle::State_Sunken;
//            }
//        }
//    }
//    if (event->type() == QEvent::MouseButtonRelease)
//    {
//        QMouseEvent* e =(QMouseEvent*)event;

//        if (m_btns.contains(index))
//        {
//            QPair<QStyleOptionButton*, QStyleOptionButton*>* btns = m_btns.value(index);
//            if (btns->first->rect.contains(e->x(), e->y()))
//            {
//                btns->first->state &= (~QStyle::State_Sunken);
//                showMsg(tr('btn1 column %1').arg(index.column()));
//            } else if(btns->second->rect.contains(e->x(), e->y()))
//            {
//                btns->second->state &= (~QStyle::State_Sunken);
//                showMsg(tr('btn2 row %1').arg(index.row()));
//            }
//        }
//    }
//}

