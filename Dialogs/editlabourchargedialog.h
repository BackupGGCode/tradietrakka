#ifndef EDITLABOURCHARGEDIALOG_H
#define EDITLABOURCHARGEDIALOG_H

#include "dbasectrl.h"

#include <QDialog>

class QSqlTableModel;
class QDataWidgetMapper;

namespace Ui {
    class EditLabourChargeDialog;
}

class EditLabourChargeDialog : public QDialog
{
    Q_OBJECT

    DBaseCtrl *dbCtrl;
    QSqlTableModel *model;
    QDataWidgetMapper *mapper;

    QModelIndex labourIndex;

    enum {
        Labour_ID = 0,
        Labour_name = 1,
        Labour_desc = 2,
        Labour_rate = 3
    };

public:
    explicit EditLabourChargeDialog(DBaseCtrl *dbCtrl, QModelIndex labourIndex, QWidget *parent = 0);
    ~EditLabourChargeDialog();

private:
    Ui::EditLabourChargeDialog *ui;

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
};

#endif // EDITLABOURCHARGEDIALOG_H
