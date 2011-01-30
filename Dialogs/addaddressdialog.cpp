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


#include "addaddressdialog.h"
#include "ui_addaddressdialog.h"

#include <QRegExp>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QDataWidgetMapper>
#include <QSqlRelationalDelegate>

/**
  * Constructor
  * @param dbCtrl, the handel to the database controller
  * @param addressID, the field ID of the current address to be edited, if -1
  * create a new db row and inset into that
  */
AddAddressDialog::AddAddressDialog(DBaseCtrl *dbCtrl, int addressID , QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddAddressDialog)
{
    ui->setupUi(this);

    setWindowTitle("Address Details");
    this->dbCtrl = dbCtrl;
    this->addressID = addressID;
    this->addressModel = dbCtrl->getAddressModel();
    initRegex();
    this->stateRelation = addressModel->relationModel(DBaseCtrl::Address_State);

    ui->stateCombo->setModel(stateRelation);
    ui->stateCombo->setModelColumn(stateRelation->fieldIndex("name"));

    addressMapper = new QDataWidgetMapper(this);
    addressMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    addressMapper->setModel(addressModel);
    addressMapper->setItemDelegate(new QSqlRelationalDelegate(this));

    addressMapper->addMapping(ui->cityEdit, DBaseCtrl::Address_City);
    addressMapper->addMapping(ui->stateCombo, DBaseCtrl::Address_State);
    addressMapper->addMapping(ui->unitEdit, DBaseCtrl::Address_UnitNum);
    addressMapper->addMapping(ui->suburbEdit, DBaseCtrl::Address_Suburb);
    addressMapper->addMapping(ui->streetEdit, DBaseCtrl::Address_Street);
    addressMapper->addMapping(ui->poBoxNumEdit, DBaseCtrl::Address_POBox);
    addressMapper->addMapping(ui->postcodeEdit, DBaseCtrl::Address_Postcode);
    addressMapper->addMapping(ui->streetNumEdit, DBaseCtrl::Address_HouseNum);

    if(addressID != -1)
    {
        QSqlRecord record;
        for(int row = 0; row < addressModel->rowCount(); row++)
        {
            record = addressModel->record(row);
            if(record.value(DBaseCtrl::Address_ID).toInt() == addressID)
            {
                addressMapper->setCurrentIndex(row);
                break;
            }
        }
        if(ui->poBoxNumEdit->text().isEmpty())
            ui->streetGroup->setChecked(true);
        else
            ui->poBoxGroup->setChecked(true);
    }
    else
    {
        int row = addressModel->rowCount();
        addressModel->insertRow(row);
        addressModel->submitAll();

        addressMapper->setCurrentIndex(row);

        ui->stateCombo->setCurrentIndex(0);

        ui->cityEdit->clear();
        ui->unitEdit->clear();
        ui->streetEdit->clear();
        ui->unitEdit->setFocus();
        ui->poBoxNumEdit->clear();
        ui->postcodeEdit->clear();
        ui->streetNumEdit->clear();
    }

    ui->poBoxGroup->setChecked(ui->poBoxNumEdit->text().length());
}

void AddAddressDialog::initRegex()
{
    QRegExp poBoxReg("([0-9]){6}");
    ui->poBoxNumEdit->setValidator(new QRegExpValidator(poBoxReg, this));

    QRegExp numReg("([0-9]){5}");
    ui->streetNumEdit->setValidator(new QRegExpValidator(numReg, this));
    ui->unitEdit->setValidator(new QRegExpValidator(numReg, this));

    QRegExp postCodeReg("([0-9]){4}");
    ui->postcodeEdit->setValidator(new QRegExpValidator(postCodeReg, this));

}

/**
  * If the ok button is clicked, submit everything to the model
  */
void AddAddressDialog::on_buttonBox_accepted()
{
    if(ui->poBoxGroup->isChecked())
    {
        ui->streetEdit->clear();
        ui->unitEdit->clear();
        ui->streetNumEdit->clear();
    }
    else
        ui->poBoxNumEdit->clear();

    addressMapper->submit();
    addressID = addressModel->record(addressModel->rowCount() - 1).value("id").toInt();
    done(addressID);
}

void AddAddressDialog::closeEvent(QCloseEvent *)
{
    on_buttonBox_rejected();
}

/**
  * If the cancel button is clicked, and the address is not valid
  * delete the row that was inserted, else just revert any changes to the mapper
  */
void AddAddressDialog::on_buttonBox_rejected()
{
    if(addressID != -1)
        addressModel->revertRow(addressMapper->currentIndex());

    addressMapper->revert();
    done(-1);
}

/**
  * Destructor
  */
AddAddressDialog::~AddAddressDialog()
{
    delete ui;
}

/**
  * Untick the POBox group if the Street group is clicked
  */
void AddAddressDialog::on_streetGroup_toggled(bool checked)
{
    ui->poBoxGroup->setChecked(!checked);
}

/**
  *Untick the Street group if the POBox group is clicked
  */
void AddAddressDialog::on_poBoxGroup_toggled(bool checked)
{
    ui->streetGroup->setChecked(!checked);
}
