#ifndef INVOICEOPENDIALOG_H
#define INVOICEOPENDIALOG_H

#include <QDialog>

namespace Ui {
    class InvoiceOpenDialog;
}

class InvoiceOpenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InvoiceOpenDialog(QWidget *parent = 0);
    ~InvoiceOpenDialog();

private:
    Ui::InvoiceOpenDialog *ui;
};

#endif // INVOICEOPENDIALOG_H
