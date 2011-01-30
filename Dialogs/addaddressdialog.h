/*******************************************************************************
  Copyright (C) 2010  Craig Hume

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


#ifndef ADDADDRESSDIALOG_H
#define ADDADDRESSDIALOG_H

#include "dbasectrl.h"

#include <QDialog>

class QSqlTableModel;
class QDataWidgetMapper;
class QSqlRelationalTableModel;

namespace Ui {
    class AddAddressDialog;
}

class AddAddressDialog : public QDialog
{
    Q_OBJECT

    DBaseCtrl *dbCtrl;
    QSqlTableModel *stateRelation;
    QDataWidgetMapper *addressMapper;
    QSqlRelationalTableModel *addressModel;

    int companyID, addressID;

//    enum
//    {
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
    explicit AddAddressDialog(DBaseCtrl *, int, QWidget *parent = 0);
    ~AddAddressDialog();

private:
    Ui::AddAddressDialog *ui;
    void initRegex();
    void closeEvent(QCloseEvent *);

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_poBoxGroup_toggled(bool );
    void on_streetGroup_toggled(bool );
};

#endif // ADDADDRESSDIALOG_H
