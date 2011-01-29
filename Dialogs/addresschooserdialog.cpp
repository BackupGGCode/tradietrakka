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


#include "addresschooserdialog.h"
#include "ui_addresschooserdialog.h"
#include <QSqlRecord>
#include <QDebug>

AddressChooserDialog::AddressChooserDialog(DBaseCtrl *dbCtrl, contactType type, int ID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddressChooserDialog)
{
    ui->setupUi(this);

    QString entityName = "";
    QSqlTableModel *entityModel;

    if(type == Business)
        entityModel = dbCtrl->getCompanyModel();
    else
        entityModel = dbCtrl->getPersonModel();

    QSqlRecord record;
    for(int row = 0; row < entityModel->rowCount(); row++)
    {
        record = entityModel->record(row);
        if(record.value("id").toInt() == ID)
        {
            if(type == Business)
                entityName = record.value(DBaseCtrl::Company_Name).toString();
            else
            {
                entityName = record.value(DBaseCtrl::Person_FName).toString();
                entityName += " " + record.value(DBaseCtrl::Person_LName).toString();
            }
            break;
        }
    }

    QString title = "<center><h3><b>" + entityName +
                    "</b> has more than one address.</h3></center>"
                    "<h4>Please select the one you want for the postal address.</h4>";

    ui->addressTitle->setText(title);

    if(type == Business)
        addressModel = dbCtrl->getCompanyAddressesModel(ID);
    else
        addressModel = dbCtrl->getPersonAddressModel(ID);


    ui->addressView->setModel(addressModel);

    ui->addressView->hideColumn(DBaseCtrl::Address_ID);
    ui->addressView->verticalHeader()->hide();
    ui->addressView->horizontalHeader()->setStretchLastSection(true);
    ui->addressView->resizeColumnsToContents();

    ui->addressView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->addressView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->addressView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->addressView->selectRow(0);
}

AddressChooserDialog::~AddressChooserDialog()
{
    delete ui;
}

void AddressChooserDialog::on_buttonBox_accepted()
{
    int addressID = addressModel->record(
            ui->addressView->selectionModel()->currentIndex().row()).value("id").toInt();

    done(addressID);
}
