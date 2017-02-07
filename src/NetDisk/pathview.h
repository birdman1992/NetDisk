#ifndef PATHVIEW_H
#define PATHVIEW_H

#include <QWidget>

namespace Ui {
class PathView;
}

class PathView : public QWidget
{
    Q_OBJECT

public:
    explicit PathView(QWidget *parent = 0);
    ~PathView();

private:
    Ui::PathView *ui;
};

#endif // PATHVIEW_H
