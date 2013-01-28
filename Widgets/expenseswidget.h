/*******************************************************************************

  TradieTrakka - Easy to use small business management software
  Copyright (C) 2010 Steven Tucker

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

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
    void on_expensePriceEdit_lostFocus();
    void on_expensePriceEdit_returnPressed();
    void on_expenseGstChkBox_clicked();
    void on_expensePriceEdit_textEdited(const QString);
};

#endif // EXPENSESWIDGET_H
