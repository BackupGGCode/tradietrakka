#include "materialtableview.h"
#include "csqltablemodel.h"
#include <QKeyEvent>
#include <QSqlTableModel>
#include <QDebug>
#include <QSqlError>

MaterialTableView::MaterialTableView(QWidget *parent) :
    QTableView(parent)
{
}

void MaterialTableView::keyPressEvent(QKeyEvent *event)
{
    QTableView::keyPressEvent(event);
    emit keyPress(event);
}

void MaterialTableView::mousePressEvent(QMouseEvent *event)
{
    QTableView::mousePressEvent(event);

    if(event->button() == Qt::RightButton)
        emit rightClick();
}
