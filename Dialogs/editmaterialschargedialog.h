#ifndef EDITMATERIALSCHARGEDIALOG_H
#define EDITMATERIALSCHARGEDIALOG_H

#include <QDialog>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlField>

#include "addressdialog.h"
#include "dbasectrl.h"

namespace Ui {
    class EditMaterialsChargeDialog;
}

class EditMaterialsChargeDialog : public QDialog
{
    Q_OBJECT

    QSqlTableModel *model;
    QModelIndex index;
    QSqlRecord record;

public:
    explicit EditMaterialsChargeDialog(DBaseCtrl *dbCtrl, QModelIndex index, QWidget *parent = 0);
    ~EditMaterialsChargeDialog();

private:
    Ui::EditMaterialsChargeDialog *ui;

private slots:
    void on_buttonBox_accepted();
};

#endif // EDITMATERIALSCHARGEDIALOG_H
