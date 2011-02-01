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
