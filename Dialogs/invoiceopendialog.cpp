#include "invoiceopendialog.h"
#include "ui_invoiceopendialog.h"

InvoiceOpenDialog::InvoiceOpenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InvoiceOpenDialog)
{
    ui->setupUi(this);
}

InvoiceOpenDialog::~InvoiceOpenDialog()
{
    delete ui;
}
