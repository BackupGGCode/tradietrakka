#ifndef MATERIALTABLEVIEW_H
#define MATERIALTABLEVIEW_H

#include <QTableView>
//#include <QKeyEvent>

class MaterialTableView : public QTableView
{
    Q_OBJECT

    int ident;

public:
    explicit MaterialTableView(QWidget *parent = 0);

    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

signals:
    void keyPress(QKeyEvent *);
    void rightClick();

};

#endif // MATERIALTABLEVIEW_H
