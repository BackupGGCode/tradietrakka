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

#ifndef COMPANYDIALOG_H
#define COMPANYDIALOG_H

#include "dbasectrl.h"
#include "contactdialog.h"

#include <QDialog>
#include <QSqlRecord>

class QModelIndex;
class QSqlTableModel;
class QDataWidgetMapper;
class QItemSelectionModel;
class QSortFilterProxyModel;

namespace Ui {
    class CompanyDialog;
}

class CompanyDialog : public QDialog
{
    Q_OBJECT

    int companyID;
    DBaseCtrl *dbCtrl;
    QSqlRecord companyRecord;
    QSqlTableModel *companyModel;
    QModelIndex companyModelIndex;
    QDataWidgetMapper *companyMapper;
    QSqlQueryModel *companyAddressModel, *companyContactsModel;
    QItemSelectionModel *addressSelectionModel, *contactSelectionModel;
    QSortFilterProxyModel *companyAddressProxyModel, *companyContactProxyModel;

//    enum
//    {
//        Company_Id = 0,
//        Company_Name = 1,
//        Company_Phone = 2,
//        Company_Fax = 3,
//        Company_Email = 4,
//        Company_WWW = 5,
//        Company_ABN = 6
//    };

public:
    explicit CompanyDialog(DBaseCtrl*, QModelIndex rowIndex, QWidget *parent = 0);
    ~CompanyDialog();

private:
    Ui::CompanyDialog *ui;

    void initContacts();
    void initAddresses();
    void refreshContacts();
    void refreshAddresses();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_addAddressButton_clicked();
    void on_addContactsButton_clicked();
    void on_editAddressButton_clicked();
    void on_removeAddressButton_clicked();
    void on_removeContactsButton_clicked();
};

#endif // COMPANYDIALOG_H
