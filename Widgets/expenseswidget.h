#ifndef EXPENSESWIDGET_H
#define EXPENSESWIDGET_H

#include "dbasectrl.h"

#include <QWidget>

class QSqlTableModel;
class QDataWidgetMapper;

namespace Ui {
    class ExpensesWidget;
}

class ExpensesWidget : public QWidget
{
    Q_OBJECT

    QSqlTableModel *model;
    QDataWidgetMapper *mapper;
    QModelIndex expenseIndex;
    int expenseID;

public:
    explicit ExpensesWidget(DBaseCtrl *dbCtrl, int expenseID, QWidget *parent = 0);
    ~ExpensesWidget();

    int getExpenseID() { return  expenseID; }
    void submitData();

private:
    Ui::ExpensesWidget *ui;


private slots:
    void on_expenseTotalEdit_lostFocus();
};

#endif // EXPENSESWIDGET_H
