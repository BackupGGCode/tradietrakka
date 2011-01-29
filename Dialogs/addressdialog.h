#ifndef ADDRESSDIALOG_H
#define ADDRESSDIALOG_H

#include "dbasectrl.h"

#include <QDialog>

class QItemSelectionModel;
class QSortFilterProxyModel;
class QSqlRelationalTableModel;
class QModelIndex;

namespace Ui {
    class AddressDialog;
}

class AddressDialog : public QDialog
{
    Q_OBJECT

    DBaseCtrl *dbCtrl;
    QSqlRelationalTableModel *addressModel;
    QSortFilterProxyModel *addressProxyModel;
    QItemSelectionModel *addressSelectionModel;

//    enum{
//        Address_ID = 0,
//        Address_UnitNum = 1,
//        Address_HouseNum = 2,
//        Address_POBox = 3,
//        Address_Street = 4,
//        Address_Suburb = 5,
//        Address_Postcode = 6,
//        Address_City = 7,
//        Address_State = 8
//    };

public:
    explicit AddressDialog(DBaseCtrl *dbCtrl, QWidget *parent = 0);
    ~AddressDialog();

private:
    Ui::AddressDialog *ui;

private slots:
    void scrollToSelection();
    void on_addButton_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_editButton_clicked();
    void on_viewButton_clicked();
    void on_searchEdit_textEdited(QString );
    void on_searchCombo_currentIndexChanged(int index);
};

#endif // ADDRESSDIALOG_H
