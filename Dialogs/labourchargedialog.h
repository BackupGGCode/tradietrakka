#ifndef LABOURCHARGEDIALOG_H
#define LABOURCHARGEDIALOG_H

#include <QDialog>

#include "dbasectrl.h"

class QSqlTableModel;
class QItemSelectionModel;
class QSortFilterProxyModel;
namespace Ui {
    class LabourChargeDialog;
}

class LabourChargeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LabourChargeDialog(DBaseCtrl* dbCtrl, QWidget *parent = 0);
    ~LabourChargeDialog();

private:
    Ui::LabourChargeDialog *ui;

    QSqlTableModel *labourModel;
    QSortFilterProxyModel *labourProxyModel;
    QItemSelectionModel *labourSelectionModel;

    DBaseCtrl *dbCtrl;

    int currColumn;

private slots:

    void on_deleteButton_clicked();
    void on_labourTable_doubleClicked(QModelIndex index);
    void on_comboBox_currentIndexChanged(QString );
    void on_lineEdit_textEdited(QString );
    void on_editButton_clicked();
    void on_addButton_clicked();
    void sortByColumn(int );
};

#endif // LABOURCHARGEDIALOG_H
