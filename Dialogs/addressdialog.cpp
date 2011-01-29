#include "addressdialog.h"
#include "ui_addressdialog.h"

#include "addaddressdialog.h"

#include <QDebug>
#include <QSqlRecord>
#include <QModelIndex>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>
#include <QSqlRelationalTableModel>

/**
  * Constructor
  * @param dbCtrl, handel to the database controller
  */
AddressDialog::AddressDialog(DBaseCtrl *dbCtrl, QWidget *parent):
    QDialog(parent),
    ui(new Ui::AddressDialog)
{
    ui->setupUi(this);

    setWindowTitle("Addresses");

    this->dbCtrl = dbCtrl;
    this->addressModel = dbCtrl->getAddressModel();

    addressProxyModel = new QSortFilterProxyModel(this);
    addressProxyModel->setSourceModel(addressModel);

    addressProxyModel->sort(2, Qt::AscendingOrder);
    addressProxyModel->setDynamicSortFilter(true);

    ui->addressList->setModel(addressProxyModel);

    addressSelectionModel = ui->addressList->selectionModel();

    addressProxyModel->setHeaderData(DBaseCtrl::Address_City, Qt::Horizontal, "City");
    addressProxyModel->setHeaderData(DBaseCtrl::Address_Suburb, Qt::Horizontal, "Suburb");
    addressProxyModel->setHeaderData(DBaseCtrl::Address_Street, Qt::Horizontal, "Street");
    addressProxyModel->setHeaderData(DBaseCtrl::Address_POBox, Qt::Horizontal, "PO Box");

    ui->addressList->hideColumn(DBaseCtrl::Address_ID);
    ui->addressList->hideColumn(DBaseCtrl::Address_UnitNum);
    ui->addressList->hideColumn(DBaseCtrl::Address_HouseNum);
    ui->addressList->hideColumn(DBaseCtrl::Address_Postcode);
    ui->addressList->hideColumn(DBaseCtrl::Address_State);

    ui->addressList->verticalHeader()->hide();
    ui->addressList->horizontalHeader()->setStretchLastSection(true);
    ui->addressList->resizeColumnsToContents();

    ui->addressList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->addressList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->addressList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->addressList->selectRow(0);

    connect(ui->addressList->horizontalHeader(),
            SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this,
            SLOT(scrollToSelection()));
}

/**
  * Destructor
  */
AddressDialog::~AddressDialog()
{
    delete ui;
}

/**
  * Add button, when clicked it opens AddAddressDialog for adding a new address
  */
void AddressDialog::on_addButton_clicked()
{
    AddAddressDialog ad(dbCtrl, -1, this);
    int addressID = ad.exec();
    addressModel->select();

    QSqlRecord record;
    for(int row = 0; row < addressModel->rowCount(); row++)
    {
        record = addressModel->record(row);
        if(record.value("id").toInt() == addressID)
        {
            ui->addressList->selectRow(addressProxyModel->mapFromSource(
                    addressModel->index(row, 0)).row());
            break;
        }
    }

}

/**
  * Edit button, opens the AddAddressDialog for editing an address
  */
void AddressDialog::on_editButton_clicked()
{
    QSqlRecord record = addressModel->record(
            addressProxyModel->mapToSource(
                    addressSelectionModel->currentIndex()).row());

    int addressID = record.value(DBaseCtrl::Address_ID).toInt();

    QModelIndex currentIndex = addressSelectionModel->currentIndex();

    AddAddressDialog ad(dbCtrl, addressID, this);
    ad.exec();

    ui->addressList->selectRow(currentIndex.row());

}

/**
  * View button ############### TO DO ##############
  */
void AddressDialog::on_viewButton_clicked()
{

}

/**
  * Search/Filter, when text is entered change the filter on the proxy model
  */
void AddressDialog::on_searchEdit_textEdited(QString text)
{
    // If personal select the text from starting with the entered text
    QString newStr = QString("^%1.*").arg(text);
    int col = DBaseCtrl::Address_Street;

    // Use the combo to decide which column to filter in
    switch (ui->searchCombo->currentIndex())
    {
//    case 0:
//        col = DBaseCtrl::Address_Street;
//        break;
    case 1:
        col = DBaseCtrl::Address_City;
        break;
    case 2:
        col = DBaseCtrl::Address_POBox;
        break;
    case 3:
        col = DBaseCtrl::Address_Suburb;
        break;
    }

    addressProxyModel->setFilterRegExp(QRegExp(newStr, Qt::CaseInsensitive));
    addressProxyModel->setFilterKeyColumn(col);
    addressProxyModel->sort(1, Qt::AscendingOrder);
}

/**
  * Search combo, when changed it calls the search/filter
  */
void AddressDialog::on_searchCombo_currentIndexChanged(int)
{
    on_searchEdit_textEdited(ui->searchEdit->text());
}


/**
  * Ok button, return the addressID of the current address
  */
void AddressDialog::on_buttonBox_accepted()
{
    QModelIndex index = addressSelectionModel->currentIndex();
    int addressID = addressModel->record(
            addressProxyModel->mapToSource(index).row()).value("id").toInt();

    done(addressID);
}

/**
  * Cancel button, return -1 for ignore
  */
void AddressDialog::on_buttonBox_rejected()
{
    done(-1);
}

/**
  * Make sure the selection stays in view
  */
void AddressDialog::scrollToSelection()
{
    ui->addressList->scrollTo(addressSelectionModel->currentIndex());
}
