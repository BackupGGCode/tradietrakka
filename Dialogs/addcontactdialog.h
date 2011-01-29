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

#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include "addressdialog.h"
#include "dbasectrl.h"

#include <QDialog>

class QSqlTableModel;
class QDataWidgetMapper;

namespace Ui {
    class AddContactDialog;
}

class AddContactDialog : public QDialog
{
    Q_OBJECT

    DBaseCtrl *dbCtrl;

    int personID;
    bool edit;
    QModelIndex personModelIndex;
    QSqlTableModel *personModel;
    QSqlTableModel *personalAddressModel;
    QDataWidgetMapper *personMapper;
    QSortFilterProxyModel *personAddressProxyModel;
    QSqlQueryModel *personAddressModel;
    QItemSelectionModel *addressSelectionModel;

//    enum
//    {
//        Person_ID = 0,
//        Person_FName = 1,
//        Person_LName = 2,
//        Person_HomePH = 3,
//        Person_OfficePH = 4,
//        Person_MobilePH = 5,
//        Person_FAX = 6,
//        Person_Email = 7,
//        Person_ABN = 8
//    };

public:
    explicit AddContactDialog(DBaseCtrl *dbCtrl, int personID, QWidget *parent = 0);
    ~AddContactDialog();


private:
    Ui::AddContactDialog *ui;

    void initAddresses();
    void refreshAddresses();
    void initRegex();

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_removeAddressButton_clicked();
    void on_editAddressButton_clicked();
    void on_addAddressButton_clicked();
    void scrollToSelection();
};

#endif // ADDCONTACTDIALOG_H
