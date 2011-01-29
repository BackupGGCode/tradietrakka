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

#include "addcontactdialog.h"
#include "addaddressdialog.h"
#include "ui_addcontactdialog.h"

//#include <QSqlQuery>
//#include <QSqlError>
#include <QDebug>
//#include <QRegExp>
#include <QSqlRecord>
#include <QMessageBox>
#include <QSqlField>

//#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QDataWidgetMapper>

#define STREET_COL 1
#define STREET_COL_WIDTH 250

AddContactDialog::AddContactDialog(DBaseCtrl *dbCtrl, int personID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddContactDialog)
{
    ui->setupUi(this);

    setWindowTitle("Personal Contact");

    this->dbCtrl = dbCtrl;
//    this->personModelIndex = index;
    this->personModel = dbCtrl->getPersonModel();
    this->personID = personID;

    personMapper = new QDataWidgetMapper(this);
    personMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    personMapper->setModel(personModel);

    personMapper->addMapping(ui->firstNameEdit, DBaseCtrl::Person_FName);
    personMapper->addMapping(ui->lastNameEdit, DBaseCtrl::Person_LName);
    personMapper->addMapping(ui->homePHEdit, DBaseCtrl::Person_HomePH);
    personMapper->addMapping(ui->mobileEdit, DBaseCtrl::Person_MobilePH);
    personMapper->addMapping(ui->officePHEdit, DBaseCtrl::Person_OfficePH);
    personMapper->addMapping(ui->faxEdit, DBaseCtrl::Person_FAX);
    personMapper->addMapping(ui->emailEdit, DBaseCtrl::Person_Email);
    personMapper->addMapping(ui->abnEdit, DBaseCtrl::Person_ABN);

    edit = false;
    if(personID != -1)
    {
        edit = true;
        QSqlRecord record;
        for(int row = 0; row < personModel->rowCount(); row++)
        {
            record = personModel->record(row);
            if(record.value(DBaseCtrl::Person_ID).toInt() == personID)
            {
                personMapper->setCurrentIndex(row);
                break;
            }
        }

    }
    else
    {
        int row = personModel->rowCount();
        personModel->insertRow(row);
        personMapper->setCurrentIndex(row);

        ui->firstNameEdit->clear();
        ui->lastNameEdit->clear();
        ui->homePHEdit->clear();
        ui->mobileEdit->clear();
        ui->officePHEdit->clear();
        ui->faxEdit->clear();
        ui->emailEdit->clear();
        ui->abnEdit->clear();

    }

    personAddressModel = dbCtrl->getPersonAddressModel(personID);

    initAddresses();
    initRegex();

    connect(ui->addressList->horizontalHeader(),
            SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this,
            SLOT(scrollToSelection()));
}


void AddContactDialog::initAddresses()
{
    personAddressProxyModel = new QSortFilterProxyModel(this);
    personAddressModel->sort(1, Qt::AscendingOrder);

    refreshAddresses();

    ui->addressList->setModel(personAddressProxyModel);

    addressSelectionModel = ui->addressList->selectionModel();

    ui->addressList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->addressList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->addressList->verticalHeader()->hide();
    ui->addressList->setColumnHidden(0, true);

    ui->addressList->horizontalHeader()->setStretchLastSection(true);
    ui->addressList->setSortingEnabled(true);

    ui->addressList->setColumnWidth(STREET_COL, STREET_COL_WIDTH);

    ui->addressList->selectRow(0);
}

void AddContactDialog::refreshAddresses()
{
    personAddressModel = dbCtrl->getPersonAddressModel(personID);
    personAddressProxyModel->setSourceModel(personAddressModel);
}


void AddContactDialog::on_addAddressButton_clicked()
{
    if(!edit && (ui->firstNameEdit->text().length() || ui->lastNameEdit->text().length()))
    {
        personMapper->submit();
        personID = personModel->record(personModel->rowCount() - 1).value("id").toInt();
    }
    else if(!ui->firstNameEdit->text().length())
    {
        QMessageBox::warning(this, "Connot Add Address",
                    "Connot add an address to nobody!?!\nEnter a name first");

        return;
    }

    AddressDialog ad(dbCtrl, this);
    int addressID = ad.exec();

    if(addressID != -1)
    {
        dbCtrl->insertAddressOfPerson(personID, addressID);
        refreshAddresses();
    }

    for(int row = 0; row < personAddressModel->rowCount(); row++)
    {
        QSqlRecord record = personAddressModel->record(row);
        if(record.value("id").toInt() == addressID)
        {
            QModelIndex newIndex = personAddressProxyModel->mapFromSource(
                    personAddressModel->index(row, 1));

            ui->addressList->selectRow(newIndex.row());
            break;
        }
    }
}

void AddContactDialog::on_editAddressButton_clicked()
{
    if(personAddressProxyModel->rowCount() > 0)
    {
        QModelIndex index = addressSelectionModel->currentIndex();

        int addressID = personAddressModel->record(
                personAddressProxyModel->mapToSource(
                addressSelectionModel->currentIndex()).row()).value("id").toInt();

        AddAddressDialog ad(dbCtrl, addressID, this);
        ad.exec();

        refreshAddresses();

        ui->addressList->selectRow(index.row());
    }
}

void AddContactDialog::on_removeAddressButton_clicked()
{
    if(personAddressProxyModel->rowCount() > 0)
    {
        int button = QMessageBox::warning(this, "Remove This Address?",
                             "Are you sure you want to remove this address?",
                             QMessageBox::Yes, QMessageBox::No);

        if(button == QMessageBox::Yes)
        {
            QModelIndex index = personAddressProxyModel->mapToSource(
                    addressSelectionModel->currentIndex());

            int addressID = personAddressModel->record(
                    index.row()).field("id").value().toInt();
            dbCtrl->removeAddressOfPerson(addressID, personID);
        }

        refreshAddresses();
        ui->addressList->selectRow(0);
    }
}

void AddContactDialog::on_buttonBox_accepted()
{
    if(ui->firstNameEdit->text().length() ||
       ui->lastNameEdit->text().length())
    {
        personMapper->submit();
        int newPersonID = personModel->record(
                personModel->rowCount() -1).value("id").toInt();

        done(newPersonID);
    }
    else
    {
        done(-1);
    }
}

void AddContactDialog::on_buttonBox_rejected()
{
    if(!edit && personID != -1)
    {
        personModel->removeRow(personModel->rowCount() - 1);
        dbCtrl->removeAllAddressesOfPerson(personID);
    }

    personMapper->revert();
    done(-1);

}

void AddContactDialog::initRegex()
{
    QRegExp phoneReg("([0-9]){16}");
    ui->homePHEdit->setValidator(new QRegExpValidator(phoneReg, this));
    ui->officePHEdit->setValidator(new QRegExpValidator(phoneReg, this));
    ui->mobileEdit->setValidator(new QRegExpValidator(phoneReg, this));
    ui->faxEdit->setValidator(new QRegExpValidator(phoneReg, this));

    QRegExp abnReg("([0-9]){10}");
    ui->abnEdit->setValidator(new QRegExpValidator(abnReg, this));

    QRegExp emailReg("[a-zA-Z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[a-zA-Z0-9"
                     "!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-zA-Z0-9](?:[a-zA-Z0-9-]"
                     "*[a-zA-Z0-9])?\\.)+[a-zA-Z0-9](?:[a-zA-Z0-9-]*[a-zA-Z0-9])?");
    ui->emailEdit->setValidator(new QRegExpValidator(emailReg, this));
}

/**
  * Make sure the selection stays in view
  */
void AddContactDialog::scrollToSelection()
{
    ui->addressList->scrollTo(addressSelectionModel->currentIndex());
}

AddContactDialog::~AddContactDialog()
{
    delete ui;
}
