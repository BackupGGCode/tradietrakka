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

#include "invoicewidget.h"
#include "ui_invoicewidget.h"

#include "contactdialog.h"
#include "finddialog.h"
#include "addresschooserdialog.h"

#include <QPainter>
#include <QFontMetrics>

#include <QMenu>
#include <QtDebug>
#include <QSqlError>
#include <QSqlField>

/**
  *
  */
InvoiceWidget::InvoiceWidget(DBaseCtrl *dbCtrl, int invoiceID, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InvoiceWidget)
{
    ui->setupUi(this);
    this->dbCtrl = dbCtrl;
    this->invoiceID = invoiceID;
    entityChanged = descChanged =  dateChanged = false;

    invoiceModel = dbCtrl->getInvoiceModel();
    contactName = new QString();

    initButtonMenu();

    if(invoiceID != -1)
    {
        loadInvoice();
        getLastRowData();
        connect(materialsOnInvoiceModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                                 this, SLOT(cellChanged(QModelIndex, QModelIndex)));

        connect(labourOnInvoiceModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SLOT(cellChangedLabour(QModelIndex,QModelIndex)));

    }
    sumTotals();

    locked = false;

    int status = invoiceModel->data(invoiceModel->index(invoiceIndex.row(), DBaseCtrl::Invoice_Status)).toInt();
    status--;
    ui->paymentCombo->setCurrentIndex((Status)status);

    connect(ui->materialsView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(getLastRowData()));
}

InvoiceWidget::Status InvoiceWidget::getStatus()
{
    return status;
}

void InvoiceWidget::sumTotals()
{
    double sum = 0.0;

    for(int i = 0; i < materialsOnInvoiceModel->rowCount() - 1; i++)
        sum += materialsOnInvoiceModel->data(
                materialsOnInvoiceModel->index(i, DBaseCtrl::MoI_VTotal),
                Qt::DisplayRole).toDouble();

    for(int i = 0; i < labourOnInvoiceModel->rowCount() - 1; i++)
        sum += labourOnInvoiceModel->data(
                labourOnInvoiceModel->index(i, DBaseCtrl::LoI_VTotal),
                Qt::DisplayRole).toDouble();


    int gst = (sum / 11.0) * 100;
    double transGST = gst / 100.00;
    ui->gstAmount->setText(QString::number(transGST, 'f', 2));
    ui->totalAmountLabel->setText(QString::number(sum, 'f', 2));
}

void InvoiceWidget::getLastRowData()
{
    tmpName = materialsOnInvoiceModel->record(materialsOnInvoiceModel->rowCount() -1).field(2).value().toString();
    tmpBrand = materialsOnInvoiceModel->record(materialsOnInvoiceModel->rowCount() -1).field(3).value().toString();
    tmpQuantity = materialsOnInvoiceModel->record(materialsOnInvoiceModel->rowCount() -1).field(4).value().toString();
    tmpPrice = materialsOnInvoiceModel->record(materialsOnInvoiceModel->rowCount() -1).field(5).value().toString();
}

void InvoiceWidget::loadInvoice()
{
        QSqlRecord record;

        for(int row = 0; row < invoiceModel->rowCount(); row++)
        {
            invoiceIndex = invoiceModel->index(row, 0);
            record = invoiceModel->record(invoiceIndex.row());
            if(record.value(DBaseCtrl::Invoice_ID).toInt() == invoiceID)
                break;
        }

        addressID = record.value(DBaseCtrl::Invoice_Address).toInt();

        entityID = record.value(DBaseCtrl::Invoice_EntityId).toInt();
        entityType = record.value(DBaseCtrl::Invoice_EntityType).toInt();


        if(entityType == Company)
            parseContactInfo(Company, entityID, addressID);
        else
            parseContactInfo(Person, entityID, addressID);

        //Add the invoice number
        ui->invoiceNumID->setText(
                record.value(DBaseCtrl::Invoice_ID).toString());

        //Add the description
        ui->descriptionEdit->setPlainText(
                record.value(DBaseCtrl::Invoice_Details).toString());


        resizeDesc();

        connect(ui->descriptionEdit, SIGNAL(textChanged()), this, SLOT(resizeDesc()));

        //Add date from db

        QDate invoiceDate = QDate::fromString(record.value(
            DBaseCtrl::Invoice_IDate).toString(), DATE_FORMAT);
        ui->dateEdit->setDate(invoiceDate);

        QDate dueDate = QDate::fromString(record.value(
                DBaseCtrl::Invoice_DueDate).toString(), DATE_FORMAT);
        ui->dueDateEdit->setDate(dueDate);

        QDate paidDate = QDate::fromString(record.value(
                DBaseCtrl::Invoice_PaidDate).toString(), DATE_FORMAT);
        ui->dateEdit_2->setDate(paidDate);

        //fetch materials on the quote
        loadMaterials();

        //fetch labour on the quote
        loadLabour();
}

void InvoiceWidget::loadMaterials()
{
    materialsOnInvoiceModel = dbCtrl->getMaterialsOnInvoiceModel(invoiceID);
    materialsOnInvoiceModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    materialsOnInvoiceModel->setCols(DBaseCtrl::MoI_QTY,
                                   DBaseCtrl::MoI_Price,
                                   DBaseCtrl::MoI_VTotal);

    ui->materialsView->verticalHeader()->setVisible(false);
    ui->materialsView->setModel(materialsOnInvoiceModel);
    materialsSelection = ui->materialsView->selectionModel();

    ui->materialsView->hideColumn(DBaseCtrl::MoI_ID);
    ui->materialsView->hideColumn(DBaseCtrl::MoI_InvoiceID);

    ui->materialsView->setMinimumHeight(200);
    ui->materialsView->setMaximumHeight(200);


    connect(ui->materialsView, SIGNAL(keyPress(QKeyEvent*)), this, SLOT(viewKeyPressed(QKeyEvent*)));
    connect(ui->materialsView, SIGNAL(rightClick()), this, SLOT(materialsRightClick()));


    materialsOnInvoiceModel->insertRow(materialsOnInvoiceModel->rowCount());

    materialsOnInvoiceModel->submit();

    // Set Column widths for materials
    ui->materialsView->setColumnWidth(DBaseCtrl::MoI_Name, 224);
    ui->materialsView->setColumnWidth(DBaseCtrl::MoI_Brand, 224);
    ui->materialsView->setColumnWidth(DBaseCtrl::MoI_QTY, 98);
    ui->materialsView->setColumnWidth(DBaseCtrl::MoI_Price, 100);
    ui->materialsView->setColumnWidth(DBaseCtrl::MoI_VTotal, 100);

    resizeMaterials();
}

void InvoiceWidget::loadLabour()
{
    labourOnInvoiceModel = dbCtrl->getLabourOnInvoiceModel(invoiceID);
    labourOnInvoiceModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    labourOnInvoiceModel->setCols(DBaseCtrl::LoI_Hours,
                                   DBaseCtrl::LoI_Rate,
                                   DBaseCtrl::LoI_VTotal);

    ui->labourView->verticalHeader()->setVisible(false);
    ui->labourView->setModel(labourOnInvoiceModel);

    labourSelection = ui->labourView->selectionModel();
    ui->labourView->hideColumn(DBaseCtrl::LoI_ID);
    ui->labourView->hideColumn(DBaseCtrl::LoI_InvoiceID);

    ui->labourView->setMinimumHeight(200);
    ui->labourView->setMaximumHeight(200);

    connect(ui->labourView, SIGNAL(keyPress(QKeyEvent*)),
            SLOT(viewKeyPressedLabour(QKeyEvent*)));

    connect(ui->labourView, SIGNAL(rightClick()), SLOT(labourRightClick()));


    labourOnInvoiceModel->insertRow(labourOnInvoiceModel->rowCount());

    labourOnInvoiceModel->submit();

    ui->labourView->setColumnWidth(DBaseCtrl::LoI_Name, 224);
    ui->labourView->setColumnWidth(DBaseCtrl::LoI_Description, 224);
    ui->labourView->setColumnWidth(DBaseCtrl::LoI_Hours, 98);
    ui->labourView->setColumnWidth(DBaseCtrl::LoI_Rate, 100);
    ui->labourView->setColumnWidth(DBaseCtrl::LoI_VTotal, 100);


    resizeLabour();
}

void InvoiceWidget::viewKeyPressedLabour(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        if(labourSelection->currentIndex().row() == labourOnInvoiceModel->rowCount() - 1 &&
           labourSelection->currentIndex().column() == DBaseCtrl::LoI_VTotal)
        {
            labourOnInvoiceModel->setData(labourOnInvoiceModel->index(
                    labourOnInvoiceModel->rowCount() - 1, 1), invoiceID);

            if(!labourOnInvoiceModel->submitAll())
            {
                qDebug("Submit failed");
                qDebug() << labourOnInvoiceModel->lastError().text();
            }
            else
            {
                labourOnInvoiceModel->insertRow(labourOnInvoiceModel->rowCount());
                labourSelection->setCurrentIndex(labourOnInvoiceModel->index(
                                        labourOnInvoiceModel->rowCount() -1, DBaseCtrl::LoI_Name),
                                        QItemSelectionModel::Select);

                resizeLabour();
            }
        }
        ui->labourView->setFocus();
        sumTotals();
    }
}

void InvoiceWidget::contactsPersonal()
{
    ContactDialog cd(ContactDialog::Personal, dbCtrl, this);
    int personID = cd.exec();

    entityChanged = true;

    if(personID != -1)
    {
       parseContactInfo(Person, personID);
    }
}

void InvoiceWidget::updateContact()
{
    if(entityChanged)
    {
        entityChanged = false;

        invoiceModel->setData(invoiceModel->index(invoiceIndex.row(), DBaseCtrl::Invoice_EntityId),
                            QVariant(entityID));
        invoiceModel->setData(invoiceModel->index(invoiceIndex.row(), DBaseCtrl::Invoice_EntityType),
                            QVariant(entityType));
        invoiceModel->setData(invoiceModel->index(invoiceIndex.row(), DBaseCtrl::Invoice_Address),
                            QVariant(addressID));

        if(!invoiceModel->submitAll())
        {
            qDebug("quoteModel Failed to commit entity");
            qDebug() << invoiceModel->lastError().text();
        }
    }

    if(dateChanged)
    {
        dateChanged = false;

        invoiceModel->setData(invoiceModel->index(invoiceIndex.row(), DBaseCtrl::Invoice_IDate),
                            QVariant(ui->dateEdit->date().toString(DATE_FORMAT)));

        invoiceModel->setData(invoiceModel->index(invoiceIndex.row(), DBaseCtrl::Invoice_DueDate),
                              QVariant(ui->dueDateEdit->date().toString(DATE_FORMAT)));

        if(!invoiceModel->submitAll())
        {
            qDebug("quoteModel Failed to commit date");
            qDebug() << invoiceModel->lastError().text();
        }
    }

    if(descChanged)
    {
        descChanged = false;
        invoiceModel->setData(invoiceModel->index(invoiceIndex.row(), DBaseCtrl::Invoice_Details),
                            QVariant(ui->descriptionEdit->toPlainText()));

        if(!invoiceModel->submitAll())
        {
            qDebug("quoteModel Failed to commit description");
            qDebug() << invoiceModel->lastError().text();
        }
    }
}

void InvoiceWidget::parseContactInfo(ContactType contactType, int contactID, int addressID)
{
    QString addressString = "";
    entityID = contactID;
    entityType = contactType;

    QSqlRecord addressRecord;

    if(contactType == Company)
    {
        contactModel = dbCtrl->getCompanyModel();
    }
    else
    {
        contactModel = dbCtrl->getPersonModel();
    }

    QSqlRecord record;
    for(int row = 0; row < contactModel->rowCount(); row++)
    {
        record = contactModel->record(row);
        if(record.value("id").toInt() == contactID)
        {
            if(contactType == Company)
                addressString += record.value(DBaseCtrl::Company_Name).toString() + "\n";
            else
                addressString += record.value(DBaseCtrl::Person_FName).toString() + ", " +
                                 record.value(DBaseCtrl::Person_LName).toString() + "\n";
        }
        *contactName = addressString;
    }

    QSqlQueryModel *contactAddressModel;
    addressModel = dbCtrl->getAddressModel();

    if(contactType == Company)
        contactAddressModel = dbCtrl->getCompanyAddressesModel(contactID);
    else
        contactAddressModel = dbCtrl->getPersonAddressModel(contactID);

    if(addressID == -1)
    {
        if(contactAddressModel->rowCount() > 1)
        {
            if(contactType == Company)
            {
                AddressChooserDialog ac(dbCtrl, AddressChooserDialog::Business, contactID, this);
                addressID = ac.exec();
            }
            else
            {
                AddressChooserDialog ac(dbCtrl, AddressChooserDialog::Personal, contactID, this);
                addressID = ac.exec();
            }
        }
        else
        {
            addressID = contactAddressModel->record(0).value("id").toInt();
        }
    }

    this->addressID = addressID;

    if(addressID > 0)
    {
        for(int row = 0; row < addressModel->rowCount(); row++)
        {
            addressRecord = addressModel->record(row);
            if(addressRecord.value("id").toInt() == addressID)
                break;
        }

        QString poBox = addressRecord.value(DBaseCtrl::Address_POBox).toString();
        QString unitNum = addressRecord.value(DBaseCtrl::Address_UnitNum).toString();
        QString streetNum = addressRecord.value(DBaseCtrl::Address_HouseNum).toString();
        QString postCode = addressRecord.value(DBaseCtrl::Address_Postcode).toString();
        QString street = addressRecord.value(DBaseCtrl::Address_Street).toString();
        QString suburb = addressRecord.value(DBaseCtrl::Address_Suburb).toString();
        QString city = addressRecord.value(DBaseCtrl::Address_City).toString();
        QString state  = addressRecord.value(DBaseCtrl::Address_State).toString();

        if(!poBox.isEmpty())
        {
            addressString += "   PO Box " + poBox + "\n";
        }
        else
        {
            if(!unitNum.isEmpty())
                addressString += "   " + unitNum + "/";
            else
                addressString += "   ";

            addressString += streetNum + " ";
            addressString += street + "\n";
        }

        addressString += "   " + suburb + ", " + postCode + "\n";
        addressString += "   " + city + "\n";
        addressString += "   " + state;
    }
    ui->contactsText->setText(addressString);

    updateContact();
}

void InvoiceWidget::labourRightClick()
{
    QMenu labourRightClickMenu(ui->labourView);

    QAction *findLabourAction = new QAction("Find Labour", addButtonMenu);
    QAction *deleteLabourRow = new QAction("Delete Labour", addButtonMenu);
    labourRightClickMenu.addAction(findLabourAction);
    labourRightClickMenu.addAction(deleteLabourRow);

    connect(findLabourAction, SIGNAL(triggered()), this, SLOT(findLabour()));
    connect(deleteLabourRow, SIGNAL(triggered()), this, SLOT(deleteLabour()));

    labourRightClickMenu.popup(QCursor::pos());
    labourRightClickMenu.exec();

}

void InvoiceWidget::resizeLabour()
{
    if(labourOnInvoiceModel->rowCount() > 1)
    {
        int tableHeight = ui->labourView->rowHeight(1)
                          * (labourOnInvoiceModel->rowCount() + 1)
                          - (ui->labourView->rowHeight(1) - 2
                          - ui->labourView->horizontalHeader()->height());

        ui->labourView->setMaximumHeight(tableHeight);
        ui->labourView->setMinimumHeight(tableHeight);
    }
    else
    {
        ui->labourView->setMinimumHeight(56);
        ui->labourView->setMaximumHeight(56);
    }
}

void InvoiceWidget::viewKeyPressed(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        if(materialsSelection->currentIndex().row() == materialsOnInvoiceModel->rowCount() -1 &&
           materialsSelection->currentIndex().column() == DBaseCtrl::MoI_VTotal)
        {
            materialsOnInvoiceModel->setData(materialsOnInvoiceModel->index(
                    materialsOnInvoiceModel->rowCount() -1, 1), invoiceID);

            if(!materialsOnInvoiceModel->submitAll())
            {
                qDebug("Submit failed");
                qDebug() << materialsOnInvoiceModel->lastError().text();
            }
            else
            {
                materialsOnInvoiceModel->insertRow(materialsOnInvoiceModel->rowCount());
                materialsSelection->setCurrentIndex(materialsOnInvoiceModel->index(
                                        materialsOnInvoiceModel->rowCount() -1, DBaseCtrl::MoI_Name),
                                        QItemSelectionModel::Select);

                resizeMaterials();
            }
        }
        ui->materialsView->setFocus();
        sumTotals();
    }
}

void InvoiceWidget::initButtonMenu()
{
    addButtonMenu = new QMenu(ui->contactsButton);

    QAction *companyContactAction = new QAction("Company Contact", addButtonMenu);
    QAction *personalContactAction = new QAction("Personal Contact", addButtonMenu);

    addButtonMenu->addAction(companyContactAction);
    addButtonMenu->addAction(personalContactAction);

    ui->contactsButton->setMenu(addButtonMenu);

    connect(companyContactAction, SIGNAL(triggered()),
            this, SLOT(contactsCompany()));
    connect(personalContactAction, SIGNAL(triggered()),
            this, SLOT(contactsPersonal()));
}

void InvoiceWidget::contactsCompany()
{
    ContactDialog cd(ContactDialog::Business, dbCtrl, this);
    int companyID = cd.exec();

    entityChanged = true;

    if(companyID != -1)
        parseContactInfo(Company, companyID);
}

/**
  *
  */
//void InvoiceWidget::initImages(int ID)
//{
//    /**
//      *If there is a receipt for the invoice then init paid and date
//      *Else check date of issue for invoice < 30 days, init past due
//      */

//    if(/*hasReceipt(ID)*/true)
//    {
//        invoiceStamp = QImage(":/PaidIcon");
//        QFont paidFont("Times", 17, QFont::Bold);
//        QPainter datePainter(&invoiceStamp);
//        datePainter.setFont(paidFont);

//        //Replace with function
//        QString paidDate("15 / 07 / 1979");


//        QFontMetrics dateMetric(paidFont);
//        QRect dateRect = dateMetric.boundingRect(paidDate);

//        int x = 42 + abs(dateRect.x()) + ((120 - dateRect.width()) / 2);
//        int y = 20 + abs(dateRect.y()) + ((24 - dateRect.height()) / 2);

//        datePainter.drawText(x, y, paidDate);
//        setReadOnly(true);

//    }
//    else if(/*invoiceDate < 30days ago*/false)
//    {
//        invoiceStamp = QImage(":/PastDueIcon");
//    }

//    ui->imageLabel->setPixmap(QPixmap::fromImage(invoiceStamp));
//}

/**
  *
  */
void InvoiceWidget::setReadOnly(bool enabled)
{
    //check for a recept if there is a receipt set everything as readonly and break

    if(readOnly)
    {
        enabled = false;
        ui->editingWidget->setVisible(enabled);

    }

    ui->contactsButton->setEnabled(enabled);

    if(enabled)
    {
        ui->lockButton->setText("Lock Invoice");
    }
    else
    {
        ui->lockButton->setText("Unlock Invoice");
    }
}

/**
  *
  */
InvoiceWidget::~InvoiceWidget()
{
    delete contactName;
    delete ui;
}


void InvoiceWidget::cellChanged(QModelIndex first, QModelIndex )
{
    if(ui->materialsView->currentIndex() != first)
    {
        materialsOnInvoiceModel->insertTotalCol();
        materialsSelection->setCurrentIndex(first, QItemSelectionModel::Select);

        materialsOnInvoiceModel->setData(materialsOnInvoiceModel->index(materialsOnInvoiceModel->rowCount() -1, 2), tmpName);
        materialsOnInvoiceModel->setData(materialsOnInvoiceModel->index(materialsOnInvoiceModel->rowCount() -1, 3), tmpBrand);
        materialsOnInvoiceModel->setData(materialsOnInvoiceModel->index(materialsOnInvoiceModel->rowCount() -1, 4), tmpQuantity);
        materialsOnInvoiceModel->setData(materialsOnInvoiceModel->index(materialsOnInvoiceModel->rowCount() -1, 5), tmpPrice);
    }

    if(first.column() == DBaseCtrl::MoI_QTY || first.column() == DBaseCtrl::MoI_Price)
        sumTotals();
}

void InvoiceWidget::cellChangedLabour(QModelIndex first, QModelIndex )
{
    if(ui->labourView->currentIndex() != first)
    {
        labourOnInvoiceModel->insertTotalCol();
        labourSelection->setCurrentIndex(first, QItemSelectionModel::Select);

        labourOnInvoiceModel->setData(labourOnInvoiceModel->index(labourOnInvoiceModel->rowCount() -1, 2), tmpName);
        labourOnInvoiceModel->setData(labourOnInvoiceModel->index(labourOnInvoiceModel->rowCount() -1, 3), tmpBrand);
        labourOnInvoiceModel->setData(labourOnInvoiceModel->index(labourOnInvoiceModel->rowCount() -1, 4), tmpQuantity);
        labourOnInvoiceModel->setData(labourOnInvoiceModel->index(labourOnInvoiceModel->rowCount() -1, 5), tmpPrice);
    }

    if(first.column() == DBaseCtrl::LoI_Hours || first.column() == DBaseCtrl::LoI_Rate)
        sumTotals();
}

void InvoiceWidget::resizeDesc()
{
    ui->descriptionEdit->setMaximumHeight(ui->descriptionEdit->fontMetrics().height()
                                          * ui->descriptionEdit->document()->lineCount()
                                          + ui->descriptionEdit->fontMetrics().lineSpacing());
    ui->descriptionEdit->setMinimumHeight(ui->descriptionEdit->fontMetrics().height()
                                          * ui->descriptionEdit->document()->lineCount()
                                          + ui->descriptionEdit->fontMetrics().lineSpacing());
    descChanged = true;
    updateContact();
}

void InvoiceWidget::resizeMaterials()
{
    if(materialsOnInvoiceModel->rowCount() > 1)
    {
        ui->materialsView->setMaximumHeight(ui->materialsView->rowHeight(1)
                                            * (materialsOnInvoiceModel->rowCount() + 1)
                                            - (ui->materialsView->rowHeight(1) - 2 - ui->materialsView->horizontalHeader()->height()));
        ui->materialsView->setMinimumHeight(ui->materialsView->rowHeight(1)
                                            * (materialsOnInvoiceModel->rowCount() + 1)
                                            - (ui->materialsView->rowHeight(1) - 2 - ui->materialsView->horizontalHeader()->height()));
    } else {
        ui->materialsView->setMaximumHeight(56);
        ui->materialsView->setMinimumHeight(56);
    }
}

void InvoiceWidget::materialsRightClick()
{
    QMenu testMenu(this);

    QAction *findMaterialAction = new QAction("Find Material", addButtonMenu);
    QAction *deleteMaterialRow = new QAction("Delete Material", addButtonMenu);
    testMenu.addAction(findMaterialAction);
    testMenu.addAction(deleteMaterialRow);


    connect(findMaterialAction, SIGNAL(triggered()), this, SLOT(findMaterial()));
    connect(deleteMaterialRow, SIGNAL(triggered()), this, SLOT(deleteMaterial()));

    testMenu.popup(QCursor::pos());
    testMenu.exec();
}

void InvoiceWidget::findMaterial()
{
    FindDialog materialDialog(FindDialog::Materials, dbCtrl, this);
    int id = materialDialog.exec();

    if(id == -1) return;

    // Retrieve record for selected id
    QModelIndex matIndex;
    QSqlRecord tmpRow;
    QSqlTableModel *matModel = dbCtrl->getMaterialsModel();

    for(int i = 0; i < matModel->rowCount(); i++)
    {
        matIndex = matModel->index(i, 0);
        tmpRow = matModel->record(matIndex.row());
        if(tmpRow.value(DBaseCtrl::Mat_ID).toInt() == id)
            break;
    }

    matIndex = materialsOnInvoiceModel->index(materialsSelection->currentIndex().row(), DBaseCtrl::MoI_Name);
    materialsOnInvoiceModel->setData(matIndex, tmpRow.value(DBaseCtrl::Mat_Name).toString());

    matIndex = materialsOnInvoiceModel->index(materialsSelection->currentIndex().row(), DBaseCtrl::MoI_Brand);
    materialsOnInvoiceModel->setData(matIndex, tmpRow.value(DBaseCtrl::Mat_Brand).toString());

    matIndex = materialsOnInvoiceModel->index(materialsSelection->currentIndex().row(), DBaseCtrl::MoI_QTY);
    materialsOnInvoiceModel->setData(matIndex, 1);

    matIndex = materialsOnInvoiceModel->index(materialsSelection->currentIndex().row(), DBaseCtrl::MoI_Price);
    materialsOnInvoiceModel->setData(matIndex, tmpRow.value(DBaseCtrl::Mat_Price).toString());

    QModelIndex tmp = materialsOnInvoiceModel->index(materialsSelection->currentIndex().row(), DBaseCtrl::MoI_VTotal);
    materialsSelection->setCurrentIndex(tmp, QItemSelectionModel::Select);
    QKeyEvent *enterEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    viewKeyPressed(enterEvent);

    materialsSelection->setCurrentIndex(materialsOnInvoiceModel->index(
            materialsSelection->currentIndex().row(), DBaseCtrl::MoI_Name),
                                        QItemSelectionModel::Select);
    sumTotals();
}

void InvoiceWidget::deleteMaterial()
{
    if(materialsSelection->currentIndex().row() == materialsOnInvoiceModel->rowCount() - 1)
        return;

    if(materialsOnInvoiceModel->rowCount() > 1)
    {
        materialsOnInvoiceModel->removeRow(materialsSelection->currentIndex().row());
        materialsOnInvoiceModel->insertTotalCol();
        materialsOnInvoiceModel->submit();

        materialsOnInvoiceModel->insertRow(materialsOnInvoiceModel->rowCount() - 1);
    }
    resizeMaterials();
    sumTotals();
}

void InvoiceWidget::findLabour()
{
    FindDialog labourDialog(FindDialog::Labour, dbCtrl, this);
    int labourID = labourDialog.exec();

    if(labourID == -1)
        return;

    QModelIndex labIndex;
    QSqlRecord tmpRow;
    QSqlTableModel *labModel = dbCtrl->getLabourModel();

    for(int i = 0; i < labModel->rowCount(); i++)
    {
        labIndex = labModel->index(i, 0);
        tmpRow = labModel->record(labIndex.row());
        if(tmpRow.value(DBaseCtrl::Labour_ID).toInt() == labourID)
            break;
    }

    labIndex = labourOnInvoiceModel->index(labourSelection->currentIndex().row(), DBaseCtrl::LoI_Name);
    labourOnInvoiceModel->setData(labIndex, tmpRow.value(DBaseCtrl::Labour_Name).toString());

    labIndex = labourOnInvoiceModel->index(labourSelection->currentIndex().row(), DBaseCtrl::LoI_Description);
    labourOnInvoiceModel->setData(labIndex, tmpRow.value(DBaseCtrl::Labour_Desc));

    labIndex = labourOnInvoiceModel->index(labourSelection->currentIndex().row(), DBaseCtrl::LoI_Hours);
    labourOnInvoiceModel->setData(labIndex, 1);

    labIndex = labourOnInvoiceModel->index(labourSelection->currentIndex().row(), DBaseCtrl::LoI_Rate);
    labourOnInvoiceModel->setData(labIndex, tmpRow.value(DBaseCtrl::Labour_Rate));

    QModelIndex tmp = labourOnInvoiceModel->index(labourSelection->currentIndex().row(), DBaseCtrl::LoI_VTotal);
    labourSelection->setCurrentIndex(tmp, QItemSelectionModel::Select);
    QKeyEvent *enterEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    viewKeyPressedLabour(enterEvent);

    labourSelection->setCurrentIndex(labourOnInvoiceModel->index(
            labourSelection->currentIndex().row(), DBaseCtrl::LoI_Name),
                                     QItemSelectionModel::Select);
    sumTotals();
}

void InvoiceWidget::deleteLabour()
{
    if(labourSelection->currentIndex().row() == labourOnInvoiceModel->rowCount() - 1)
        return;

    if(labourOnInvoiceModel->rowCount() > 1)
    {
        labourOnInvoiceModel->removeRow(labourSelection->currentIndex().row());
        labourOnInvoiceModel->insertTotalCol();
        labourOnInvoiceModel->submit();
        labourOnInvoiceModel->insertRow(labourOnInvoiceModel->rowCount() - 1);
    }
    resizeLabour();
    sumTotals();
}

void InvoiceWidget::on_dateEdit_dateChanged(QDate )
{
    dateChanged = true;
    updateContact();
}

void InvoiceWidget::on_dueDateEdit_dateChanged(QDate)
{
    dateChanged = true;
    updateContact();
    emit updateOverDueNotice();
}

void InvoiceWidget::lockInvoice()
{
    locked = true;
    ui->lockButton->setText("Unlock Invoice");

    ui->scrollAreaWidgetContents->setEnabled(false);
}

void InvoiceWidget::unlockInvoice()
{
    locked = false;
    ui->lockButton->setText("Lock Invoice");

    ui->scrollAreaWidgetContents->setEnabled(true);
}

void InvoiceWidget::on_lockButton_clicked()
{
    if(locked)
    {
        unlockInvoice();
    } else {
        lockInvoice();
    }
}

void InvoiceWidget::on_paymentCombo_currentIndexChanged(int index)
{
    index++;
    status = (Status)index;
    invoiceModel->setData(invoiceModel->index(invoiceIndex.row(), DBaseCtrl::Invoice_Status), (int)status);
    invoiceModel->submit();

    if(index == 3)
    {
        ui->dateEdit_2->setEnabled(true);
    } else {
        ui->dateEdit_2->setEnabled(false);
    }

    emit updateOverDueNotice();
}


void InvoiceWidget::on_dateEdit_2_dateChanged(QDate date)
{
    invoiceModel->setData(invoiceModel->index(invoiceIndex.row(), DBaseCtrl::Invoice_PaidDate),
                          date.toString(DATE_FORMAT));
}
