#ifndef MATERIALPRICESDIALOG_H
#define MATERIALPRICESDIALOG_H

#include <QDialog>

#include "dbasectrl.h"

class QSqlTableModel;
class QItemSelectionModel;
class QSortFilterProxyModel;

namespace Ui {
    class MaterialPricesDialog;
}

class MaterialPricesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MaterialPricesDialog(DBaseCtrl* dbCtrl, QWidget *parent = 0);
    ~MaterialPricesDialog();

private:
    Ui::MaterialPricesDialog *ui;

    QSqlTableModel *materialModel;
    QSortFilterProxyModel *materialProxyModel;
    QItemSelectionModel *materialselectionModel;

    DBaseCtrl *dbCtrl;

private slots:
    void on_deleteButton_clicked();
    void on_materialsTable_doubleClicked(QModelIndex );
    void on_searchCombo_currentIndexChanged(QString );
    void on_searchEdit_textEdited(QString );
    void on_addButton_clicked();
    void on_editButton_clicked();
    void scrollToSelection();
};

#endif // MATERIALPRICESDIALOG_H
