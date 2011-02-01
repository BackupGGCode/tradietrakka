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

#include "finddialog.h"
#include "ui_finddialog.h"

#include <QDate>
#include <QSqlError>
#include <QSqlField>

#define QUOTENUM_COL_WIDTH 130
#define QDATE_COL_WIDTH 80
#define QDETAILS_COL_WIDTH 250
#define QCLIENT_COL_WIDTH 150

#define INVOICE_NUM_WIDTH 100
#define IDATE_ISSUE_WIDTH 100
#define IDETAILS_WIDTH 250
#define IDATE_PAID_WIDTH 100
#define ICLIETN_WIDTH 200

#define RECEIPT_NUM_WIDTH 150
#define RDATE_WIDTH 100
#define R_INVOICE_NUM_WIDTH 150

#define EX_DATE_WIDTH 100
#define EX_AMOUNT_WIDTH 100
#define EX_NAME_WIDTH 300

FindDialog::FindDialog(findType dialogType, DBaseCtrl *dbCtrl, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindDialog)
{
    ui->setupUi(this);
    this->dbCtrl = dbCtrl;
    proxyModel = new QSortFilterProxyModel;

    currType = dialogType;

    setWindowTitle("Find");

    ui->typeCombo->setVisible(false);

    switch(currType)
    {
    case Quote:
        initQuoteDialog();
        break;
    case Invoice:
        initInvoiceDialog();
        break;
    case Expense:
        initExpenseDialog();
        break;
    case Receipt:
        initReceiptDialog();
        break;
    case Materials:
        initMaterialsDialog();
        break;
    case Labour:
        initLabourDialog();
        break;
    case OverDue:
        initOverDueDialog();
        break;
    }
}

FindDialog::~FindDialog()
{
    delete ui;
}

void FindDialog::initQuoteDialog()
{
    ui->findGroup->setTitle("Find a Quote");
    ui->searchGroup->setTitle("Search for a Quote");

    ui->typeCombo->setVisible(true);

    QStringList list;
    list << "Company" << "Personal";

    ui->typeCombo->insertItems(0, list);

    companyRelationModel = new QSqlRelationalTableModel(this, dbCtrl->getDbase());
    companyRelationModel->setTable("Quote");
    companyRelationModel->setRelation(DBaseCtrl::Quote_EntityID, QSqlRelation("Company", "id", "Name"));

    companyRelationModel->setHeaderData(DBaseCtrl::Quote_ID, Qt::Horizontal, "Quote Number");
    companyRelationModel->setHeaderData(DBaseCtrl::Quote_QDate, Qt::Horizontal, "Date");
    companyRelationModel->setHeaderData(DBaseCtrl::Quote_Details, Qt::Horizontal, "Details");
    companyRelationModel->setHeaderData(DBaseCtrl::Quote_EntityID, Qt::Horizontal, "Client Name");

    companyRelationModel->setFilter("entityType = 0");

    companyRelationModel->select();    

    personRelationModel = new QSqlRelationalTableModel(this, dbCtrl->getDbase());

    personRelationModel->setTable("Quote");
    personRelationModel->setRelation(DBaseCtrl::Quote_EntityID, QSqlRelation("Person", "id", "lName"));

    personRelationModel->setHeaderData(DBaseCtrl::Quote_ID, Qt::Horizontal, "Quote Number");
    personRelationModel->setHeaderData(DBaseCtrl::Quote_QDate, Qt::Horizontal, "Date");
    personRelationModel->setHeaderData(DBaseCtrl::Quote_Details, Qt::Horizontal, "Details");
    personRelationModel->setHeaderData(DBaseCtrl::Quote_EntityID, Qt::Horizontal, "Client Name");

    personRelationModel->setFilter("entityType = 1");

    personRelationModel->select();

    proxyModel->setSourceModel(companyRelationModel);

    ui->findView->setModel(proxyModel);


    ui->findView->hideColumn(DBaseCtrl::Quote_QNum);
    ui->findView->hideColumn(DBaseCtrl::Quote_ExpiryDate);
    ui->findView->hideColumn(DBaseCtrl::Quote_Address);
    ui->findView->hideColumn(DBaseCtrl::Quote_EntityType);

    ui->findView->verticalHeader()->hide();
    ui->findView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->findView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->findView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    proxyModel->setFilterKeyColumn(DBaseCtrl::Quote_EntityType);
    proxyModel->setFilterRegExp("0");

    ui->searchCombo->addItem("Quote Number");
    ui->searchCombo->addItem("Date");
    ui->searchCombo->addItem("Details");
    ui->searchCombo->addItem("Client Name");

    changeQuoteEntityType(0);


    ui->findView->setColumnWidth(DBaseCtrl::Quote_ID, QUOTENUM_COL_WIDTH);
    ui->findView->setColumnWidth(DBaseCtrl::Quote_QDate, QDATE_COL_WIDTH);
    ui->findView->setColumnWidth(DBaseCtrl::Quote_Details, QDETAILS_COL_WIDTH);
    ui->findView->setColumnWidth(DBaseCtrl::Quote_EntityID, QCLIENT_COL_WIDTH);

    connect(ui->typeCombo, SIGNAL(currentIndexChanged(int)), SLOT(changeQuoteEntityType(int)));
}

void FindDialog::changeQuoteEntityType(int index)
{
    if(index)
    {
        proxyModel->setSourceModel(personRelationModel);
        proxyModel->setFilterKeyColumn(DBaseCtrl::Quote_EntityType);
        proxyModel->setFilterRegExp("1");

    } else {
        proxyModel->setSourceModel(companyRelationModel);
        proxyModel->setFilterKeyColumn(DBaseCtrl::Quote_EntityType);
        proxyModel->setFilterRegExp("0");
    }
    on_searchCombo_currentIndexChanged("");
}

void FindDialog::initInvoiceDialog()
{
    ui->findGroup->setTitle("Find an Invoice");
    ui->searchGroup->setTitle("Search for an Invoice");

    ui->typeCombo->setVisible(true);

    QStringList list;
    list << "Company" << "Personal";

    ui->typeCombo->insertItems(0, list);

    companyRelationModel = new QSqlRelationalTableModel(this, dbCtrl->getDbase());
    companyRelationModel->setTable("Invoice");
    companyRelationModel->setRelation(DBaseCtrl::Invoice_EntityId, QSqlRelation("Company", "id", "Name"));

    companyRelationModel->setHeaderData(DBaseCtrl::Invoice_ID, Qt::Horizontal, "Invoice Number");
    companyRelationModel->setHeaderData(DBaseCtrl::Invoice_IDate, Qt::Horizontal, "Issue Date");
    companyRelationModel->setHeaderData(DBaseCtrl::Invoice_Details, Qt::Horizontal, "Details");
    companyRelationModel->setHeaderData(DBaseCtrl::Invoice_PaidDate, Qt::Horizontal, "Paid Date");
    companyRelationModel->setHeaderData(DBaseCtrl::Invoice_EntityId, Qt::Horizontal, "Client Name");

    companyRelationModel->setFilter("entityType = 0");

    companyRelationModel->select();

    personRelationModel = new QSqlRelationalTableModel(this, dbCtrl->getDbase());
    personRelationModel->setTable("Invoice");
    personRelationModel->setRelation(DBaseCtrl::Invoice_EntityId, QSqlRelation("Person", "id", "lName"));

    personRelationModel->setHeaderData(DBaseCtrl::Invoice_ID, Qt::Horizontal, "Invoice Number");
    personRelationModel->setHeaderData(DBaseCtrl::Invoice_IDate, Qt::Horizontal, "Issue Date");
    personRelationModel->setHeaderData(DBaseCtrl::Invoice_Details, Qt::Horizontal, "Details");
    personRelationModel->setHeaderData(DBaseCtrl::Invoice_PaidDate, Qt::Horizontal, "Paid Date");
    personRelationModel->setHeaderData(DBaseCtrl::Invoice_EntityId, Qt::Horizontal, "Client Name");

    personRelationModel->setFilter("entityType = 1");

    personRelationModel->select();

    proxyModel->setSourceModel(companyRelationModel);

    ui->findView->setModel(proxyModel);

    //ui->findView->hideColumn(DBaseCtrl::Invoice_ID);
    ui->findView->hideColumn(DBaseCtrl::Invoice_INum);
    ui->findView->hideColumn(DBaseCtrl::Invoice_Address);
    ui->findView->hideColumn(DBaseCtrl::Invoice_DueDate);
    ui->findView->hideColumn(DBaseCtrl::Invoice_EntityType);
    ui->findView->hideColumn(DBaseCtrl::Invoice_Status);

    ui->findView->verticalHeader()->hide();
    ui->findView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->findView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->findView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    proxyModel->setFilterKeyColumn(DBaseCtrl::Invoice_EntityType);
    proxyModel->setFilterRegExp("0");

    ui->searchCombo->addItem("Invoice Number");
    ui->searchCombo->addItem("Issue Date");
    ui->searchCombo->addItem("Details");
    ui->searchCombo->addItem("Client Name");

    changeInvoiceEntityType(0);

    ui->findView->setColumnWidth(DBaseCtrl::Invoice_ID, INVOICE_NUM_WIDTH);
    ui->findView->setColumnWidth(DBaseCtrl::Invoice_IDate, IDATE_ISSUE_WIDTH);
    ui->findView->setColumnWidth(DBaseCtrl::Invoice_Details, IDETAILS_WIDTH);
    ui->findView->setColumnWidth(DBaseCtrl::Invoice_PaidDate, IDATE_PAID_WIDTH);
    ui->findView->setColumnWidth(DBaseCtrl::Invoice_EntityId, ICLIETN_WIDTH);

    connect(ui->typeCombo, SIGNAL(currentIndexChanged(int)), SLOT(changeInvoiceEntityType(int)));

    on_searchCombo_currentIndexChanged("Changed");
}

void FindDialog::changeInvoiceEntityType(int index)
{
    if(index)
    {
        proxyModel->setSourceModel(personRelationModel);
        //proxyModel->setFilterKeyColumn(DBaseCtrl::Invoice_EntityType);
        //proxyModel->setFilterRegExp("1");

    } else {
        proxyModel->setSourceModel(companyRelationModel);
        //proxyModel->setFilterKeyColumn(DBaseCtrl::Invoice_EntityType);
        //proxyModel->setFilterRegExp("0");
    }

    on_searchCombo_currentIndexChanged("");
}

void FindDialog::initExpenseDialog()
{
    ui->findGroup->setTitle("Find an Expense");
    ui->searchGroup->setTitle("Search for an Expense");

    proxyModel->setSourceModel(dbCtrl->getExpenseModel());
    ui->findView->setModel(proxyModel);
    ui->findView->hideColumn(DBaseCtrl::Expense_ID);
    ui->findView->hideColumn(DBaseCtrl::Expense_Qty);
    ui->findView->hideColumn(DBaseCtrl::Expense_GST);
    ui->findView->hideColumn(DBaseCtrl::Expense_Desc);
    ui->findView->verticalHeader()->hide();
    ui->findView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->findView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->findView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    proxyModel->setHeaderData(DBaseCtrl::Expense_EDate, Qt::Horizontal, "Date");
    proxyModel->setHeaderData(DBaseCtrl::Expense_Amount, Qt::Horizontal, "Amount");
    proxyModel->setHeaderData(DBaseCtrl::Expense_Name, Qt::Horizontal, "Name");

    ui->searchCombo->addItem("Date");
    ui->searchCombo->addItem("Amount");
    ui->searchCombo->addItem("Name");

    ui->findView->setColumnWidth(DBaseCtrl::Expense_EDate, EX_DATE_WIDTH);
    ui->findView->setColumnWidth(DBaseCtrl::Expense_Amount, EX_AMOUNT_WIDTH);
    ui->findView->setColumnWidth(DBaseCtrl::Expense_Name, EX_NAME_WIDTH);
}

void FindDialog::initReceiptDialog()
{
    ui->findGroup->setTitle("Find a Receipt");
    ui->searchGroup->setTitle("Search for a Receipt");

    proxyModel->setSourceModel(dbCtrl->getReceiptModel());
    ui->findView->setModel(proxyModel);
    ui->findView->hideColumn(DBaseCtrl::Receipt_Details);
    ui->findView->verticalHeader()->hide();
    ui->findView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->findView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->findView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    proxyModel->setHeaderData(DBaseCtrl::Receipt_ID, Qt::Horizontal, "Receipt Number");
    proxyModel->setHeaderData(DBaseCtrl::Receipt_RDate, Qt::Horizontal, "Date");
    proxyModel->setHeaderData(DBaseCtrl::Receipt_InvoiceID, Qt::Horizontal, "Invoice Number");

    ui->searchCombo->addItem("Receipt Number");
    ui->searchCombo->addItem("Date");
    ui->searchCombo->addItem("Invoice Number");

    ui->findView->setColumnWidth(DBaseCtrl::Receipt_ID, RECEIPT_NUM_WIDTH);
    ui->findView->setColumnWidth(DBaseCtrl::Receipt_RDate, RDATE_WIDTH);
    ui->findView->setColumnWidth(DBaseCtrl::Receipt_InvoiceID, R_INVOICE_NUM_WIDTH);
}

void FindDialog::initMaterialsDialog()
{
    ui->findGroup->setTitle("Find Materials");
    ui->searchGroup->setTitle("Search for Materials");

    proxyModel->setSourceModel(dbCtrl->getMaterialsModel());
    ui->findView->setModel(proxyModel);
    ui->findView->hideColumn(DBaseCtrl::Mat_ID);
    ui->findView->hideColumn(DBaseCtrl::Mat_Quantity);
    ui->findView->verticalHeader()->hide();
    ui->findView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->findView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->findView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    proxyModel->setHeaderData(DBaseCtrl::Mat_Name, Qt::Horizontal, "Name");
    proxyModel->setHeaderData(DBaseCtrl::Mat_Brand, Qt::Horizontal, "Brand");
    proxyModel->setHeaderData(DBaseCtrl::Mat_Description, Qt::Horizontal, "Description");
    proxyModel->setHeaderData(DBaseCtrl::Mat_Price, Qt::Horizontal, "Sell Price");
    proxyModel->setHeaderData(DBaseCtrl::Mat_PartNum, Qt::Horizontal, "Part Number");
    proxyModel->setHeaderData(DBaseCtrl::Mat_WholePrice, Qt::Horizontal, "Buy Price");

    ui->searchCombo->addItem("Name");
    ui->searchCombo->addItem("Brand");
    ui->searchCombo->addItem("Description");
    ui->searchCombo->addItem("Sell Price");
    ui->searchCombo->addItem("Part Number");
    ui->searchCombo->addItem("Buy Price");
}

void FindDialog::on_buttonBox_accepted()
{
    if(ui->findView->selectionModel()->currentIndex().isValid())
    {
        QSqlTableModel *basMod = qobject_cast<QSqlTableModel*>(proxyModel->sourceModel());

        QModelIndex sourceIndex = proxyModel->mapToSource(ui->findView->selectionModel()->currentIndex());
        int currQuoteId = basMod->record(sourceIndex.row()).field("id").value().toInt();
        done(currQuoteId);
    }
    else
        done(-1);
}

void FindDialog::closeEvent(QCloseEvent *)
{
    done(-1);
}

void FindDialog::on_buttonBox_rejected()
{
    done(-1);
}

void FindDialog::initLabourDialog()
{
    ui->findGroup->setTitle("Find a Labour Rate");
    ui->searchGroup->setTitle("Search for a Labour Rate");

    proxyModel->setSourceModel(dbCtrl->getLabourModel());
    ui->findView->setModel(proxyModel);
    ui->findView->hideColumn(DBaseCtrl::Labour_ID);
    ui->findView->verticalHeader()->hide();
    ui->findView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->findView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->findView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    proxyModel->setHeaderData(DBaseCtrl::Labour_Name, Qt::Horizontal, "Name");
    proxyModel->setHeaderData(DBaseCtrl::Labour_Desc, Qt::Horizontal, "Description");
    proxyModel->setHeaderData(DBaseCtrl::Labour_Rate, Qt::Horizontal, "Rate");

    QStringList labourList;
    labourList << "Name" << "Description" << "Rate";
    ui->searchCombo->addItems(labourList);
}

void FindDialog::initOverDueDialog()
{
    ui->findGroup->setTitle("Find an Invoice");
    ui->searchGroup->setTitle("Search for an Invoice");

    ui->typeCombo->setVisible(true);

    QStringList list;
    list << "Company" << "Personal";

    ui->typeCombo->insertItems(0, list);

    companyRelationModel = new QSqlRelationalTableModel(this, dbCtrl->getDbase());
    companyRelationModel->setTable("Invoice");
    companyRelationModel->setRelation(DBaseCtrl::Invoice_EntityId, QSqlRelation("Company", "id", "Name"));

    companyRelationModel->setHeaderData(DBaseCtrl::Invoice_ID, Qt::Horizontal, "Invoice Number");
    companyRelationModel->setHeaderData(DBaseCtrl::Invoice_IDate, Qt::Horizontal, "Issue Date");
    companyRelationModel->setHeaderData(DBaseCtrl::Invoice_Details, Qt::Horizontal, "Details");
    companyRelationModel->setHeaderData(DBaseCtrl::Invoice_PaidDate, Qt::Horizontal, "Paid Date");
    companyRelationModel->setHeaderData(DBaseCtrl::Invoice_EntityId, Qt::Horizontal, "Client Name");

    companyRelationModel->setFilter("dueDate < " + QDate::currentDate().toString(DATE_FORMAT) +
                                    " AND status < 3 AND "
                                    "entityType = 0");

    companyRelationModel->select();

    personRelationModel = new QSqlRelationalTableModel(this, dbCtrl->getDbase());
    personRelationModel->setTable("Invoice");
    personRelationModel->setRelation(DBaseCtrl::Invoice_EntityId, QSqlRelation("Person", "id", "lName"));

    personRelationModel->setHeaderData(DBaseCtrl::Invoice_ID, Qt::Horizontal, "Invoice Number");
    personRelationModel->setHeaderData(DBaseCtrl::Invoice_IDate, Qt::Horizontal, "Issue Date");
    personRelationModel->setHeaderData(DBaseCtrl::Invoice_Details, Qt::Horizontal, "Details");
    personRelationModel->setHeaderData(DBaseCtrl::Invoice_PaidDate, Qt::Horizontal, "Paid Date");
    personRelationModel->setHeaderData(DBaseCtrl::Invoice_EntityId, Qt::Horizontal, "Client Name");

    personRelationModel->setFilter("dueDate < " + QDate::currentDate().toString(DATE_FORMAT) +
                                   " AND status < 3 AND "
                                   "entityType = 1");

    personRelationModel->select();

    proxyModel->setSourceModel(companyRelationModel);

    ui->findView->setModel(proxyModel);

    ui->findView->hideColumn(DBaseCtrl::Invoice_INum);
    ui->findView->hideColumn(DBaseCtrl::Invoice_Address);
    ui->findView->hideColumn(DBaseCtrl::Invoice_DueDate);
    ui->findView->hideColumn(DBaseCtrl::Invoice_EntityType);
    ui->findView->hideColumn(DBaseCtrl::Invoice_Status);

    ui->findView->verticalHeader()->hide();
    ui->findView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->findView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->findView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //proxyModel->setFilterKeyColumn(DBaseCtrl::Invoice_EntityType);
    //proxyModel->setFilterRegExp("0");



    ui->searchCombo->addItem("Invoice Number");
    ui->searchCombo->addItem("Issue Date");
    ui->searchCombo->addItem("Details");
    ui->searchCombo->addItem("Client Name");

    changeInvoiceEntityType(0);

    connect(ui->typeCombo, SIGNAL(currentIndexChanged(int)), SLOT(changeInvoiceEntityType(int)));
}

void FindDialog::on_searchCombo_currentIndexChanged(QString )
{
    ui->searchEdit->setFocus();
    on_searchEdit_textEdited(ui->searchEdit->text());
}

void FindDialog::on_searchEdit_textEdited(QString newText)
{
    QModelIndex firstIndex = ui->findView->currentIndex();
    QModelIndex finalIndex = proxyModel->mapToSource(firstIndex);

    int col = ui->searchCombo->currentIndex();

    switch(currType)
    {
    case Quote:
        switch(col)
        {
        case 0:
            col = DBaseCtrl::Quote_ID;
            break;
        case 1:
            col = DBaseCtrl::Quote_QDate;
            break;
        case 2:
            col = DBaseCtrl::Quote_Details;
            break;
        case 3:
            col = DBaseCtrl::Quote_EntityID;
            break;
        }
        break;
    case Invoice:
        switch(col)
        {
        case 0:
            col = DBaseCtrl::Invoice_ID;
            break;
        case 1:
            col = DBaseCtrl::Invoice_IDate;
            break;
        case 2:
            col = DBaseCtrl::Invoice_Details;
            break;
        case 3:
            col = DBaseCtrl::Invoice_EntityId;
            break;
        case 4:
            col = DBaseCtrl::Invoice_Status;
            break;
        }
        break;
    case Expense:
        switch(col)
        {
        case 0:
            col = DBaseCtrl::Expense_EDate;
            break;
        case 1:
            col = DBaseCtrl::Expense_Amount;
            break;
        case 2:
            col = DBaseCtrl::Expense_Name;
            break;
        }
        break;
    case Receipt:
        switch(col)
        {
        case 0:
            col = DBaseCtrl::Receipt_ID;
            break;
        case 1:
            col = DBaseCtrl::Receipt_RDate;
            break;
        case 2:
            col = DBaseCtrl::Receipt_InvoiceID;
            break;
        }
        break;
    case Materials:
        switch(col)
        {
        case 0:
            col = DBaseCtrl::Mat_Name;
            break;
        case 1:
            col = DBaseCtrl::Mat_Brand;
            break;
        case 2:
            col = DBaseCtrl::Mat_Description;
            break;
        case 3:
            col = DBaseCtrl::Mat_Price;
            break;
        case 4:
            col = DBaseCtrl::Mat_PartNum;
            break;
        case 5:
            col = DBaseCtrl::Mat_WholePrice;
            break;
        }
        break;
    case Labour:
        col++;
        break;
    case OverDue:
        switch(col)
        {
        case 0:
            col = DBaseCtrl::Invoice_ID;
            break;
        case 1:
            col = DBaseCtrl::Invoice_IDate;
            break;
        case 2:
            col = DBaseCtrl::Invoice_Details;
            break;
        case 3:
            col = DBaseCtrl::Invoice_EntityId;
            break;
        case 4:
            col = DBaseCtrl::Invoice_Status;
            break;
        }
        break;
    default:
        col++;
    }


    QString sortString = QString("^%1.*").arg(newText);

    proxyModel->setFilterRegExp((QRegExp(sortString, Qt::CaseInsensitive)));
    proxyModel->setFilterKeyColumn(col);
    proxyModel->sort(1, Qt::AscendingOrder);

    int newIndex = proxyModel->mapFromSource(finalIndex).row();

    if(newIndex > 0)
        ui->findView->selectRow(newIndex);
    else
        ui->findView->selectRow(0);
}
