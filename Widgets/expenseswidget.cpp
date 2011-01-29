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

    mapper->setCurrentModelIndex(expenseIndex);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    ui->expenseDateEdit->setDate(QDate::fromString(model->data(
            model->index(expenseIndex.row(), DBaseCtrl::Expense_EDate)).toString(), "yyyyMMdd"));

    QRegExp numRegex("([0-9]*\\.[0-9][0-9])");

    ui->expensePriceEdit->setValidator(new QRegExpValidator(numRegex, this));
    ui->expenseGSTEdit->setValidator(new QRegExpValidator(numRegex, this));
    ui->expenseTotalEdit->setValidator(new QRegExpValidator(numRegex, this));

    double exPrice = model->data(model->index(expenseIndex.row(), DBaseCtrl::Expense_Amount)).toDouble();
    ui->expensePriceEdit->setText(QString::number(exPrice / 11.0, 'f', 2));
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

void ExpensesWidget::on_expenseTotalEdit_lostFocus()
{
    double gstAmount = ui->expenseTotalEdit->text().toDouble() / 11.0;
    ui->expenseGSTEdit->setText(QString::number(gstAmount, 'f', 2));
    ui->expensePriceEdit->setText(QString::number(gstAmount * 10, 'f', 2));
}
