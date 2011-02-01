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

#include "companydialog.h"
#include "ui_companydialog.h"

#include "addressdialog.h"
#include "addaddressdialog.h"
#include "addcontactdialog.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QDataWidgetMapper>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>

#include <QSqlField>

#define STREET_COL 1
#define STREET_COL_WIDTH 200

#define FNAME_COL 1
#define FNAME_COL_WIDTH 200
/**
  *
  */
CompanyDialog::CompanyDialog(DBaseCtrl* dbCtrl, QModelIndex index, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CompanyDialog)
{
    ui->setupUi(this);

    setWindowTitle("Company Contacts");

    this->dbCtrl = dbCtrl;
    this->companyModelIndex = index;
    this->companyModel = dbCtrl->getCompanyModel();

    companyMapper = new QDataWidgetMapper(this);
    companyMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    companyMapper->setModel(companyModel);

    companyMapper->addMapping(ui->companyNameEdit, DBaseCtrl::Company_Name);
    companyMapper->addMapping(ui->companyPhoneEdit, DBaseCtrl::Company_Phone);
    companyMapper->addMapping(ui->companyFAXEdit, DBaseCtrl::Company_Fax);
    companyMapper->addMapping(ui->companyEmailEdit, DBaseCtrl::Company_Email);
    companyMapper->addMapping(ui->companyWWWEdit, DBaseCtrl::Company_WWW);
    companyMapper->addMapping(ui->companyABNEdit, DBaseCtrl::Company_ABN);

    companyMapper->setCurrentModelIndex(companyModelIndex);

    companyID = companyModel->record(companyModelIndex.row()).value("id").toInt();

    initContacts();
    initAddresses();

    QRegExp numRegex("([0-9]){16}");

    ui->companyABNEdit->setValidator(new QRegExpValidator(numRegex, this));
    ui->companyPhoneEdit->setValidator(new QRegExpValidator(numRegex, this));
    ui->companyFAXEdit->setValidator(new QRegExpValidator(numRegex, this));
}

/**
  *
  */
void CompanyDialog::initContacts()
{
    companyContactProxyModel = new QSortFilterProxyModel(this);
    refreshContacts();
    companyContactProxyModel->setSourceModel(companyContactsModel);
    companyContactProxyModel->sort(2, Qt::AscendingOrder);

    ui->contactsList->setModel(companyContactProxyModel);

    contactSelectionModel = ui->contactsList->selectionModel();

    ui->contactsList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->contactsList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->contactsList->verticalHeader()->hide();
    ui->contactsList->setColumnHidden(0, true);

    ui->contactsList->resizeColumnsToContents();
    ui->contactsList->horizontalHeader()->setStretchLastSection(true);

    ui->contactsList->setColumnWidth(STREET_COL, STREET_COL_WIDTH);

    ui->contactsList->selectRow(0);
}

void CompanyDialog::refreshContacts()
{
    companyContactsModel = dbCtrl->getCompanyContactsModel(companyID);

    companyContactProxyModel->setSourceModel(companyContactsModel);
}

void CompanyDialog::initAddresses()
{
    companyAddressProxyModel = new QSortFilterProxyModel(this);
    companyAddressProxyModel->sort(1, Qt::AscendingOrder);

    refreshAddresses();

    ui->addressList->setModel(companyAddressProxyModel);

    addressSelectionModel = ui->addressList->selectionModel();

    ui->addressList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->addressList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->addressList->verticalHeader()->hide();
    ui->addressList->setColumnHidden(0, true);

    ui->addressList->resizeColumnsToContents();
    ui->addressList->horizontalHeader()->setStretchLastSection(true);

    ui->addressList->setColumnWidth(FNAME_COL, FNAME_COL_WIDTH);

    ui->addressList->selectRow(0);
}

void CompanyDialog::refreshAddresses()
{
    companyAddressModel = dbCtrl->getCompanyAddressesModel(companyID);

    companyAddressProxyModel->setSourceModel(companyAddressModel);
}

/**
  *
  */
void CompanyDialog::on_addContactsButton_clicked()
{
    ContactDialog cd(ContactDialog::Personal, dbCtrl, this);
    int personID = cd.exec();

    if(personID != -1)
    {
        dbCtrl->insertPersonAtCompany(companyID, personID);
        refreshContacts();
    }

    QModelIndex index = companyContactsModel->index(
            companyContactsModel->rowCount() - 1, 1);

    QModelIndex newIndex = companyContactProxyModel->mapFromSource(index);

    ui->contactsList->selectRow(newIndex.row());
}

/**
  *
  */
void CompanyDialog::on_removeContactsButton_clicked()
{
    if(companyContactProxyModel->rowCount() > 0)
    {
        int button = QMessageBox::warning(this, "Remove Contact?",
                             "Are you sure you want to remove this contact?",
                             QMessageBox::Yes, QMessageBox::No);

        if(button == QMessageBox::Yes)
        {
            QModelIndex index = companyContactProxyModel->mapToSource(
                    contactSelectionModel->currentIndex());

            int personID = companyContactsModel->record(
                    index.row()).field("id").value().toInt();

            dbCtrl->removePersonAtCompany(personID);
        }

        refreshContacts();
        ui->contactsList->selectRow(0);
    }
}

/**
  *
  */
void CompanyDialog::on_addAddressButton_clicked()
{
    AddressDialog ad(dbCtrl, this);
    int addressID = ad.exec();

    if(addressID != -1)
    {
        dbCtrl->insertAddressOfCompany(companyID, addressID);
        refreshAddresses();
    }

    for(int row = 0; row < companyAddressModel->rowCount(); row++)
    {
        QSqlRecord record = companyAddressModel->record(row);
        if(record.value("id").toInt() == addressID)
        {
            QModelIndex newIndex = companyAddressProxyModel->mapFromSource(
                    companyAddressModel->index(row, 1));

            ui->addressList->selectRow(newIndex.row());
            break;
        }
    }
}

/**
  *
  */
void CompanyDialog::on_editAddressButton_clicked()
{
    if(companyAddressProxyModel->rowCount() > 0)
    {
        QModelIndex index = addressSelectionModel->currentIndex();

        int addressID = companyAddressModel->record(
                companyAddressProxyModel->mapToSource(
                addressSelectionModel->currentIndex()).row()).value("id").toInt();

        AddAddressDialog ad(dbCtrl, addressID, this);
        ad.exec();
        refreshAddresses();

        ui->addressList->selectRow(index.row());
    }
}

/**
  *
  */
void CompanyDialog::on_removeAddressButton_clicked()
{
    if(companyAddressProxyModel->rowCount() > 0)
    {
        int button = QMessageBox::warning(this, "Remove Companies Address?",
                             "Are you sure you want to remove this address?",
                             QMessageBox::Yes, QMessageBox::No);

        if(button == QMessageBox::Yes)
        {
            QModelIndex index = companyAddressProxyModel->mapToSource(
                    addressSelectionModel->currentIndex());
            int addressID = companyAddressModel->record(
                    index.row()).field("id").value().toInt();
            dbCtrl->removeAddressOfCompany(addressID);
        }

        refreshAddresses();
        ui->addressList->selectRow(0);
    }
}

/**
  *
  */
void CompanyDialog::on_buttonBox_accepted()
{
    if(ui->companyNameEdit->text().length())
    {
       companyMapper->submit();
    }
}

/**
  *
  */
void CompanyDialog::on_buttonBox_rejected()
{
    companyMapper->revert();
}

/**
  * Destructor
  */
CompanyDialog::~CompanyDialog()
{
    delete ui;
}
