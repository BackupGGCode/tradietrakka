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

#include "expenseswidget.h"
#include "ui_expenseswidget.h"

#include <QSqlError>
#include <QSqlTableModel>
#include <QDataWidgetMapper>

ExpensesWidget::ExpensesWidget(DBaseCtrl *dbCtrl, int expenseID, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExpensesWidget)
{
    ui->setupUi(this);
    this->expenseID = expenseID;
    this->model = dbCtrl->getExpenseModel();

    QSqlRecord tmp;
    for(int i = 0; i < model->rowCount(); i++)
    {
        tmp = model->record(i);
        if(tmp.value(DBaseCtrl::Expense_ID) == expenseID)
        {
            expenseIndex = model->index(i, 0);
            break;
        }
    }

    mapper = new QDataWidgetMapper(this);

    mapper->setModel(this->model);

    mapper->addMapping(ui->expenseItemEdit, DBaseCtrl::Expense_Name);
    mapper->addMapping(ui->expenseQTYEdit, DBaseCtrl::Expense_Qty);
    mapper->addMapping(ui->expenseGSTEdit, DBaseCtrl::Expense_GST);
    mapper->addMapping(ui->expenseTotalEdit, DBaseCtrl::Expense_Amount);
    mapper->addMapping(ui->descriptionEdit, DBaseCtrl::Expense_Desc);
    mapper->addMapping(ui->expenseGstChkBox, DBaseCtrl::Expense_GST_CHCK);

    mapper->setCurrentModelIndex(expenseIndex);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    ui->expenseDateEdit->setDate(QDate::fromString(model->data(
            model->index(expenseIndex.row(), DBaseCtrl::Expense_EDate)).toString(), "yyyyMMdd"));

    QRegExp numRegex("([0-9]*\\.[0-9][0-9])");

    ui->expensePriceEdit->setValidator(new QRegExpValidator(numRegex, this));
    ui->expenseGSTEdit->setValidator(new QRegExpValidator(numRegex, this));
    ui->expenseTotalEdit->setValidator(new QRegExpValidator(numRegex, this));

    double exPrice = model->data(model->index(expenseIndex.row(), DBaseCtrl::Expense_Amount)).toDouble();
    if(ui->expenseGstChkBox->isChecked())
    {
        ui->expensePriceEdit->setText(QString::number((exPrice / 11.0) * 10, 'f', 2));
    } else {
        ui->expensePriceEdit->setText(QString::number(exPrice, 'f', 2));
    }
}

ExpensesWidget::~ExpensesWidget()
{
    delete ui;
}

void ExpensesWidget::submitData()
{
    model->setData(model->index(expenseIndex.row(), DBaseCtrl::Expense_EDate), ui->expenseDateEdit->date().toString("yyyyMMdd"));
    mapper->submit();
}

void ExpensesWidget::on_expensePriceEdit_lostFocus()
{
    on_expensePriceEdit_returnPressed();
}

void ExpensesWidget::on_expensePriceEdit_returnPressed()
{
    double gstAmount = ui->expensePriceEdit->text().toDouble() / 10.0;
    if(ui->expenseGstChkBox->isChecked())
    {
        ui->expenseGSTEdit->setText(QString::number(gstAmount, 'f', 2));
    } else {
        ui->expenseGSTEdit->setText("0.00");
    }
    ui->expenseTotalEdit->setText(QString::number(ui->expenseGSTEdit->text().toDouble() + ui->expensePriceEdit->text().toDouble(), 'f', 2));
}

void ExpensesWidget::on_expenseGstChkBox_clicked()
{
    on_expensePriceEdit_returnPressed();
}

void ExpensesWidget::on_expensePriceEdit_textEdited(const QString)
{
    on_expensePriceEdit_returnPressed();
}

void ExpensesWidget::on_expenseTotalEdit_lostFocus()
{
    on_expensePriceEdit_returnPressed();
}
