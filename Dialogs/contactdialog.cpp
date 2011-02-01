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

#include "contactdialog.h"
#include "companydialog.h"
#include "addcontactdialog.h"
#include "ui_contactdialog.h"
#include "viewcontactdialog.h"

#include <QDebug>

#include <QAction>
#include <QDialog>
#include <QSqlRecord>
#include <QModelIndex>
#include <QSqlTableModel>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>

#include <QSqlField>

#include <QInputDialog>

/**
  * Constructor
  *
  * @param contactType, enum type of the type of dialog to display (Business, Personal)
  * @param DBaseCtrl*, a handle to the DB to aquire access to all required models
  *
  */
ContactDialog::ContactDialog(contactType contType, DBaseCtrl* dbCtrl, QWidget *parent) :
        QDialog(parent), ui(new Ui::ContactDialog)
{
    ui->setupUi(this);
    
    this->dbCtrl = dbCtrl;
    this->thisContactType = contType;


    ui->contactList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->contactList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->contactList->resizeColumnsToContents();
    ui->contactList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->contactList->verticalHeader()->hide();

    ui->contactList->horizontalHeader()->setStretchLastSection(true);
    ui->contactList->setSortingEnabled(true);
    ui->contactList->setColumnWidth(1, 145);


    if(thisContactType == Business)
        initBusiness();
    else
        initPersonal();



    ui->contactList->setFocus();
    ui->contactList->selectRow(0);

    connect(ui->contactList->horizontalHeader(),
            SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this,
            SLOT(scrollToSelection()));
}

/**
  *
  */
void ContactDialog::initBusiness()
{
    setWindowTitle("Company Contacts");
    ui->addButton->setText("New Company");
    ui->searchCombo->setVisible(false);

    this->companyModel = dbCtrl->getCompanyModel();

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(companyModel);
    proxyModel->sort(1, Qt::AscendingOrder);
    proxyModel->setDynamicSortFilter(true);

    ui->contactList->setModel(proxyModel);

    selectionModel = ui->contactList->selectionModel();

    ui->contactList->hideColumn(0);
    for(int i = 2; i < 8; i++)
        ui->contactList->hideColumn(i);

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addCompanyContact()));
}

/**
  *
  */
void ContactDialog::initPersonal()
{
    setWindowTitle("Personal Contacts");
    ui->addButton->setText("Add Personal");

    this->personModel = dbCtrl->getPersonModel();

    proxyModel = new QSortFilterProxyModel;
    proxyModel->setSourceModel(personModel);
    proxyModel->sort(1, Qt::AscendingOrder);
    proxyModel->setDynamicSortFilter(true);

    ui->contactList->setModel(proxyModel);

    selectionModel = ui->contactList->selectionModel();

    ui->contactList->hideColumn(0);
    for(int i = 3; i < 9; i++)
        ui->contactList->hideColumn(i);

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addPersonalContact()));
}

/**
  * Add a new company contact
  */
void ContactDialog::addCompanyContact()
{
    bool ok;
    QString newCompanyName = QInputDialog::getText(this, "New Company Name",
                                                   "Enter the name of the company contact",
                                                   QLineEdit::Normal, "", &ok);

    if(ok && newCompanyName.length() > 1)
    {
        proxyModel->insertRow(0);
        proxyModel->setData(proxyModel->index(0,1), newCompanyName);
        proxyModel->submit();

        QModelIndex newIndex = proxyModel->mapFromSource(companyModel->index(companyModel->rowCount() - 1, 0));
        ui->contactList->selectRow(newIndex.row());

        on_editButton_clicked();
    }
}

/**
  * Add a new personal contact
  */
void ContactDialog::addPersonalContact()
{
    int oldRow = selectionModel->currentIndex().row();

    AddContactDialog ac(dbCtrl, -1, this);
    int newID = ac.exec();

    personModel->select();

    if(newID == -1)
    {
        ui->contactList->selectRow(oldRow);
    }
    else
    {
        QSqlRecord record;
        for(int row = 0; row < personModel->rowCount(); row++)
        {
            record = personModel->record(row);
            if(record.value("id").toInt() == newID)
            {
                ui->contactList->selectRow(proxyModel->mapFromSource(
                        personModel->index(row, 0)).row());
                break;
            }
        }
    }

}

/**
  * When the edit button is clicked, create a new dialog of
  * either a CompanyDialog or PersonalDialog
  */
void ContactDialog::on_editButton_clicked()
{
    if(thisContactType == Business)
    {
        if(proxyModel->rowCount() > 0)
        {
            /*
             * Store the old model index from the selection
             * model to be used after the dialog returns
             */
            QModelIndex oldIndex = selectionModel->currentIndex();
            QModelIndex index = proxyModel->mapToSource(oldIndex);
            CompanyDialog editBusinessContact(dbCtrl, index, this);
            editBusinessContact.exec();

            // Highlight the edited item
            ui->contactList->selectRow(oldIndex.row());
        }
    }
    else
    {
        if(proxyModel->rowCount() > 0)
        {
            /*
             * Store the old model index from the selection
             * model to be used after the dialog returns
             */
            QModelIndex oldIndex = selectionModel->currentIndex();

            QSqlRecord record = personModel->record(
                    proxyModel->mapToSource(
                            selectionModel->currentIndex()).row());

            int personID = record.value("id").toInt();

            AddContactDialog ac(dbCtrl, personID, this);
            ac.exec();

            ui->contactList->selectRow(oldIndex.row());
        }
    }
}

/**
  * When the view button is clicked, create a new viewContactDialog
  * and pass in the *****Record**** change to dbctrl and index
  */
void ContactDialog::on_viewButton_clicked()
{
    QModelIndex index = proxyModel->mapToSource(selectionModel->currentIndex());// ui->contactList->currentIndex());

    if(thisContactType == Business)
    {
        if(proxyModel->rowCount() > 0)
        {
            int entityID = companyModel->record(index.row()).value(DBaseCtrl::Company_Id).toInt();
            ViewContactDialog vcd(ViewContactDialog::Company, dbCtrl, entityID, this);
            vcd.exec();
        }
    }
    else
    {
        if(proxyModel->rowCount() > 0)
        {
            int entityID = personModel->record(index.row()).value(DBaseCtrl::Person_ID).toInt();
            ViewContactDialog vcd(ViewContactDialog::Personal, dbCtrl, entityID, this);
            vcd.exec();
        }
    }
}

/**
  * When the ok button is clicked
  * @return id of the current selected record for adding into the bridge table
  */
void ContactDialog::on_buttonBox_2_accepted()
{
    int ID = -1;
    if(thisContactType == Personal)
    {
        QModelIndex index = proxyModel->mapToSource(selectionModel->currentIndex());
        ID = personModel->record(index.row()).field("id").value().toInt();
    }
    else
    {
        QModelIndex index = proxyModel->mapToSource(selectionModel->currentIndex());
        ID = companyModel->record(index.row()).field("id").value().toInt();
    }

    done(ID);

}

/**
  * When the search/filter edit is edited, create a regular expression and apply
  * it to the proxyModel, used to filter the view.
  */
void ContactDialog::on_searchEdit_textEdited(QString text)
{
    QModelIndex firstIndex = ui->contactList->currentIndex();
    QModelIndex finalIndex = proxyModel->mapToSource(firstIndex);

    QString newStr;
    int col = 1;
    if(thisContactType == Business)
    {
        // If business type, select the text and either side of it
        newStr = QString("^%1.*").arg(text);
    }
    else
    {
        // If personal select the text from starting with the entered text
        newStr = QString("^%1.*").arg(text);
        // Use the combo to decide which column to filter in, First or Last Names
        col = (ui->searchCombo->currentIndex()) ? 2 : 1;
    }

    proxyModel->setFilterRegExp(QRegExp(newStr, Qt::CaseInsensitive));
    proxyModel->setFilterKeyColumn(col);
    proxyModel->sort(1, Qt::AscendingOrder);

    int newIndex = proxyModel->mapFromSource(finalIndex).row();

    if(newIndex >= 0)
        ui->contactList->selectRow(newIndex);
    else
        ui->contactList->selectRow(0);
}

/**
  * If the searchCombo is changed from First Name to Last Name or vise versa
  * reapply the search/filter by selecting the textEdit slot
  */
void ContactDialog::on_searchCombo_currentIndexChanged(int)
{
    ui->searchEdit->setFocus();
    on_searchEdit_textEdited(ui->searchEdit->text());
}


/**
  * Make sure the selection stays in view
  */
void ContactDialog::scrollToSelection()
{
    ui->contactList->scrollTo(selectionModel->currentIndex());
}

/**
  * Destructor
  */
ContactDialog::~ContactDialog()
{
    delete ui;
}
