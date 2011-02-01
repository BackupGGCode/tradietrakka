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

#include "quotewidget.h"
#include "ui_quotewidget.h"
#include "contactdialog.h"
#include "finddialog.h"
#include "addresschooserdialog.h"

#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>


/**
  * Constructor for an exsisting Quote Widget
  */
QuoteWidget::QuoteWidget(DBaseCtrl *dbCtrl, int quoteID, QWidget *parent):
        QWidget(parent),
        ui(new Ui::QuoteWidget)
{
    ui->setupUi(this);
    this->dbCtrl = dbCtrl;
    this->quoteID = quoteID;
    entityChanged = descChanged =  dateChanged = false;

    quoteModel = dbCtrl->getQuoteModel();
    contactName = new QString();
    initButtonMenu();

    if(quoteID != -1)
    {
        loadQuote();
        getLastRowData();
        connect(materialsOnQuoteModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                                 this, SLOT(cellChanged(QModelIndex, QModelIndex)));

        connect(labourOnQuoteModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SLOT(cellChangedLabour(QModelIndex,QModelIndex)));

    }
    sumTotals();

    connect(ui->materialsView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(getLastRowData()));
}

void QuoteWidget::sumTotals()
{
    double sum = 0.0;

    for(int i = 0; i < materialsOnQuoteModel->rowCount() - 1; i++)
        sum += materialsOnQuoteModel->data(
                materialsOnQuoteModel->index(i, DBaseCtrl::MoQ_VTotal),
                Qt::DisplayRole).toDouble();

    for(int i = 0; i < labourOnQuoteModel->rowCount() - 1; i++)
        sum += labourOnQuoteModel->data(
                labourOnQuoteModel->index(i, DBaseCtrl::LoQ_VTotal),
                Qt::DisplayRole).toDouble();


    int gst = (sum / 11.0) * 100;
    double transGST = gst / 100.00;

    ui->gstAmount->setText(QString::number(transGST, 'f', 2));
    ui->totalAmountLabel->setText(QString::number(sum, 'f', 2));
}

void QuoteWidget::getLastRowData()
{
    tmpName = materialsOnQuoteModel->record(materialsOnQuoteModel->rowCount() -1).field(2).value().toString();
    tmpBrand = materialsOnQuoteModel->record(materialsOnQuoteModel->rowCount() -1).field(3).value().toString();
    tmpQuantity = materialsOnQuoteModel->record(materialsOnQuoteModel->rowCount() -1).field(4).value().toString();
    tmpPrice = materialsOnQuoteModel->record(materialsOnQuoteModel->rowCount() -1).field(5).value().toString();
}

void QuoteWidget::loadQuote()
{
        QSqlRecord record;

        for(int row = 0; row < quoteModel->rowCount(); row++)
        {
            quoteIndex = quoteModel->index(row, 0);
            record = quoteModel->record(quoteIndex.row());
            if(record.value(DBaseCtrl::Quote_ID).toInt() == quoteID)
                break;
        }

        addressID = record.value(DBaseCtrl::Quote_Address).toInt();

        entityID = record.value(DBaseCtrl::Quote_EntityID).toInt();
        entityType = record.value(DBaseCtrl::Quote_EntityType).toInt();


        if(entityType == Company)
            parseContactInfo(Company, entityID, addressID);
        else
            parseContactInfo(Person, entityID, addressID);


        //Add the Quote Number
        ui->quoteNumID->setText(
                record.value(DBaseCtrl::Quote_ID).toString());

        //Add the description
        ui->descriptionEdit->setPlainText(
                record.value(DBaseCtrl::Quote_Details).toString());


        resizeDesc();

        connect(ui->descriptionEdit, SIGNAL(textChanged()), this, SLOT(resizeDesc()));

        //Add date from db
        QDate quoteDate = QDate::fromString(record.value(
            DBaseCtrl::Quote_QDate).toString(), DATE_FORMAT);
        ui->dateEdit->setDate(quoteDate);

        //fetch materials on the quote
        loadMaterials();

        //fetch labour on the quote
        loadLabour();
}

void QuoteWidget::loadMaterials()
{
    materialsOnQuoteModel = dbCtrl->getMaterialsOnQuoteModel(quoteID);
    materialsOnQuoteModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    materialsOnQuoteModel->setCols(DBaseCtrl::MoQ_QTY,
                                   DBaseCtrl::MoQ_Price,
                                   DBaseCtrl::MoQ_VTotal);

    ui->materialsView->verticalHeader()->setVisible(false);
    ui->materialsView->setModel(materialsOnQuoteModel);
    materialsSelection = ui->materialsView->selectionModel();

    ui->materialsView->hideColumn(DBaseCtrl::MoQ_ID);
    ui->materialsView->hideColumn(DBaseCtrl::MoQ_QuoteID);

    ui->materialsView->setMinimumHeight(200);
    ui->materialsView->setMaximumHeight(200);


    connect(ui->materialsView, SIGNAL(keyPress(QKeyEvent*)), this, SLOT(viewKeyPressed(QKeyEvent*)));
    connect(ui->materialsView, SIGNAL(rightClick()), this, SLOT(materialsRightClick()));


    materialsOnQuoteModel->insertRow(materialsOnQuoteModel->rowCount());

    materialsOnQuoteModel->submit();

    // Set Column widths for materials
    ui->materialsView->setColumnWidth(DBaseCtrl::MoQ_Name, 224);
    ui->materialsView->setColumnWidth(DBaseCtrl::MoQ_Brand, 224);
    ui->materialsView->setColumnWidth(DBaseCtrl::MoQ_QTY, 98);
    ui->materialsView->setColumnWidth(DBaseCtrl::MoQ_Price, 100);
    ui->materialsView->setColumnWidth(DBaseCtrl::MoQ_VTotal, 100);

    resizeMaterials();
}

void QuoteWidget::loadLabour()
{
    labourOnQuoteModel = dbCtrl->getLabourOnQuoteModel(quoteID);
    labourOnQuoteModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    labourOnQuoteModel->setCols(DBaseCtrl::LoQ_Hours,
                                   DBaseCtrl::LoQ_Rate,
                                   DBaseCtrl::LoQ_VTotal);

    ui->labourView->verticalHeader()->setVisible(false);
    ui->labourView->setModel(labourOnQuoteModel);

    labourSelection = ui->labourView->selectionModel();
    ui->labourView->hideColumn(DBaseCtrl::LoQ_ID);
    ui->labourView->hideColumn(DBaseCtrl::LoQ_QuoteID);

    ui->labourView->setMinimumHeight(200);
    ui->labourView->setMaximumHeight(200);

    connect(ui->labourView, SIGNAL(keyPress(QKeyEvent*)),
            SLOT(viewKeyPressedLabour(QKeyEvent*)));

    connect(ui->labourView, SIGNAL(rightClick()), SLOT(labourRightClick()));


    labourOnQuoteModel->insertRow(labourOnQuoteModel->rowCount());

    labourOnQuoteModel->submit();

    ui->labourView->setColumnWidth(DBaseCtrl::LoQ_Name, 224);
    ui->labourView->setColumnWidth(DBaseCtrl::LoQ_Description, 224);
    ui->labourView->setColumnWidth(DBaseCtrl::LoQ_Hours, 98);
    ui->labourView->setColumnWidth(DBaseCtrl::LoQ_Rate, 100);
    ui->labourView->setColumnWidth(DBaseCtrl::LoQ_VTotal, 100);


    resizeLabour();
}

void QuoteWidget::viewKeyPressedLabour(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        if(labourSelection->currentIndex().row() == labourOnQuoteModel->rowCount() - 1 &&
           labourSelection->currentIndex().column() == DBaseCtrl::LoQ_VTotal)
        {
            labourOnQuoteModel->setData(labourOnQuoteModel->index(
                    labourOnQuoteModel->rowCount() - 1, 1), quoteID);

            if(!labourOnQuoteModel->submitAll())
            {
                qDebug("Submit failed");
                qDebug() << labourOnQuoteModel->lastError().text();
            }
            else
            {
                labourOnQuoteModel->insertRow(labourOnQuoteModel->rowCount());
                labourSelection->setCurrentIndex(labourOnQuoteModel->index(
                                        labourOnQuoteModel->rowCount() -1, DBaseCtrl::LoQ_Name),
                                        QItemSelectionModel::Select);

                resizeLabour();
            }
        }
        ui->labourView->setFocus();
        sumTotals();
    }
}

void QuoteWidget::labourRightClick()
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

void QuoteWidget::resizeLabour()
{
    if(labourOnQuoteModel->rowCount() > 1)
    {
        int tableHeight = ui->labourView->rowHeight(1)
                          * (labourOnQuoteModel->rowCount() + 1)
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

void QuoteWidget::viewKeyPressed(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        if(materialsSelection->currentIndex().row() == materialsOnQuoteModel->rowCount() -1 &&
           materialsSelection->currentIndex().column() == DBaseCtrl::MoQ_VTotal)
        {
            materialsOnQuoteModel->setData(materialsOnQuoteModel->index(
                    materialsOnQuoteModel->rowCount() -1, 1), quoteID);

            if(!materialsOnQuoteModel->submitAll())
            {
                qDebug("Submit failed");
                qDebug() << materialsOnQuoteModel->lastError().text();
            }
            else
            {
                materialsOnQuoteModel->insertRow(materialsOnQuoteModel->rowCount());
                materialsSelection->setCurrentIndex(materialsOnQuoteModel->index(
                                        materialsOnQuoteModel->rowCount() -1, DBaseCtrl::MoQ_Name),
                                        QItemSelectionModel::Select);

                resizeMaterials();
            }
        }
        ui->materialsView->setFocus();
        sumTotals();
    }
}

void QuoteWidget::initButtonMenu()
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

void QuoteWidget::contactsCompany()
{
    ContactDialog cd(ContactDialog::Business, dbCtrl, this);
    int companyID = cd.exec();

    entityChanged = true;

    if(companyID != -1)
        parseContactInfo(Company, companyID);
}

void QuoteWidget::contactsPersonal()
{
    ContactDialog cd(ContactDialog::Personal, dbCtrl, this);
    int personID = cd.exec();

    entityChanged = true;

    if(personID != -1)
    {
       parseContactInfo(Person, personID);
    }
}

void QuoteWidget::updateContact()
{
    if(entityChanged)
    {
        entityChanged = false;

        quoteModel->setData(quoteModel->index(quoteIndex.row(), DBaseCtrl::Quote_EntityID),
                            QVariant(entityID));
        quoteModel->setData(quoteModel->index(quoteIndex.row(), DBaseCtrl::Quote_EntityType),
                            QVariant(entityType));
        quoteModel->setData(quoteModel->index(quoteIndex.row(), DBaseCtrl::Quote_Address),
                            QVariant(addressID));

        if(!quoteModel->submitAll())
        {
            qDebug("quoteModel Failed to commit entity");
            qDebug() << quoteModel->lastError().text();
        }
    }

    if(dateChanged)
    {
        dateChanged = false;
        quoteModel->setData(quoteModel->index(quoteIndex.row(), DBaseCtrl::Quote_QDate),
                            QVariant(ui->dateEdit->date().toString(DATE_FORMAT)));

        if(!quoteModel->submitAll())
        {
            qDebug("quoteModel Failed to commit date");
            qDebug() << quoteModel->lastError().text();
        }
    }

    if(descChanged)
    {
        descChanged = false;
        quoteModel->setData(quoteModel->index(quoteIndex.row(), DBaseCtrl::Quote_Details),
                            QVariant(ui->descriptionEdit->toPlainText()));

        if(!quoteModel->submitAll())
        {
            qDebug("quoteModel Failed to commit description");
            qDebug() << quoteModel->lastError().text();
        }
    }
}

void QuoteWidget::parseContactInfo(ContactType contactType, int contactID, int addressID)
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
                addressString += record.value(DBaseCtrl::Person_FName).toString() + " " +
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

        addressString += "   " + suburb;

        if(!postCode.isEmpty())
            addressString += ", " + postCode + "\n";

        addressString += "   " + city + "\n";
        addressString += "   " + state;

    }
    ui->contactsText->setText(addressString);

    //if(quoteIndex.isValid())
        updateContact();
}


QuoteWidget::~QuoteWidget()
{
    delete contactName;
    delete ui;
}

void QuoteWidget::cellChanged(QModelIndex first, QModelIndex )
{
    if(ui->materialsView->currentIndex() != first)
    {
        materialsOnQuoteModel->insertTotalCol();
        materialsSelection->setCurrentIndex(first, QItemSelectionModel::Select);

        materialsOnQuoteModel->setData(materialsOnQuoteModel->index(materialsOnQuoteModel->rowCount() -1, 2), tmpName);
        materialsOnQuoteModel->setData(materialsOnQuoteModel->index(materialsOnQuoteModel->rowCount() -1, 3), tmpBrand);
        materialsOnQuoteModel->setData(materialsOnQuoteModel->index(materialsOnQuoteModel->rowCount() -1, 4), tmpQuantity);
        materialsOnQuoteModel->setData(materialsOnQuoteModel->index(materialsOnQuoteModel->rowCount() -1, 5), tmpPrice);
    }

    if(first.column() == DBaseCtrl::MoQ_QTY || first.column() == DBaseCtrl::MoQ_Price)
        sumTotals();
}

void QuoteWidget::cellChangedLabour(QModelIndex first, QModelIndex )
{
    if(ui->labourView->currentIndex() != first)
    {
        labourOnQuoteModel->insertTotalCol();
        labourSelection->setCurrentIndex(first, QItemSelectionModel::Select);

        labourOnQuoteModel->setData(labourOnQuoteModel->index(labourOnQuoteModel->rowCount() -1, 2), tmpName);
        labourOnQuoteModel->setData(labourOnQuoteModel->index(labourOnQuoteModel->rowCount() -1, 3), tmpBrand);
        labourOnQuoteModel->setData(labourOnQuoteModel->index(labourOnQuoteModel->rowCount() -1, 4), tmpQuantity);
        labourOnQuoteModel->setData(labourOnQuoteModel->index(labourOnQuoteModel->rowCount() -1, 5), tmpPrice);
    }

    if(first.column() == DBaseCtrl::LoQ_Hours || first.column() == DBaseCtrl::LoQ_Rate)
        sumTotals();
}

void QuoteWidget::resizeDesc()
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

void QuoteWidget::resizeMaterials()
{
    if(materialsOnQuoteModel->rowCount() > 1)
    {
        ui->materialsView->setMaximumHeight(ui->materialsView->rowHeight(1)
                                            * (materialsOnQuoteModel->rowCount() + 1)
                                            - (ui->materialsView->rowHeight(1) - 2 - ui->materialsView->horizontalHeader()->height()));
        ui->materialsView->setMinimumHeight(ui->materialsView->rowHeight(1)
                                            * (materialsOnQuoteModel->rowCount() + 1)
                                            - (ui->materialsView->rowHeight(1) - 2 - ui->materialsView->horizontalHeader()->height()));
    } else {
        ui->materialsView->setMaximumHeight(56);
        ui->materialsView->setMinimumHeight(56);
    }
}

void QuoteWidget::materialsRightClick()
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

void QuoteWidget::findMaterial()
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

    matIndex = materialsOnQuoteModel->index(materialsSelection->currentIndex().row(), DBaseCtrl::MoQ_Name);
    materialsOnQuoteModel->setData(matIndex, tmpRow.value(DBaseCtrl::Mat_Name).toString());

    matIndex = materialsOnQuoteModel->index(materialsSelection->currentIndex().row(), DBaseCtrl::MoQ_Brand);
    materialsOnQuoteModel->setData(matIndex, tmpRow.value(DBaseCtrl::Mat_Brand).toString());

    matIndex = materialsOnQuoteModel->index(materialsSelection->currentIndex().row(), DBaseCtrl::MoQ_QTY);
    materialsOnQuoteModel->setData(matIndex, 1);

    matIndex = materialsOnQuoteModel->index(materialsSelection->currentIndex().row(), DBaseCtrl::MoQ_Price);
    materialsOnQuoteModel->setData(matIndex, tmpRow.value(DBaseCtrl::Mat_Price).toString());

    QModelIndex tmp = materialsOnQuoteModel->index(materialsSelection->currentIndex().row(), DBaseCtrl::MoQ_VTotal);
    materialsSelection->setCurrentIndex(tmp, QItemSelectionModel::Select);
    QKeyEvent *enterEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    viewKeyPressed(enterEvent);

    materialsSelection->setCurrentIndex(materialsOnQuoteModel->index(
            materialsSelection->currentIndex().row(), DBaseCtrl::MoQ_Name),
                                        QItemSelectionModel::Select);
    sumTotals();
}

void QuoteWidget::deleteMaterial()
{
    if(materialsSelection->currentIndex().row() == materialsOnQuoteModel->rowCount() - 1)
        return;

    if(materialsOnQuoteModel->rowCount() > 1)
    {
        materialsOnQuoteModel->removeRow(materialsSelection->currentIndex().row());
        materialsOnQuoteModel->insertTotalCol();
        materialsOnQuoteModel->submit();

        materialsOnQuoteModel->insertRow(materialsOnQuoteModel->rowCount() - 1);
    }
    resizeMaterials();
    sumTotals();
}

void QuoteWidget::findLabour()
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

    labIndex = labourOnQuoteModel->index(labourSelection->currentIndex().row(), DBaseCtrl::LoQ_Name);
    labourOnQuoteModel->setData(labIndex, tmpRow.value(DBaseCtrl::Labour_Name).toString());

    labIndex = labourOnQuoteModel->index(labourSelection->currentIndex().row(), DBaseCtrl::LoQ_Description);
    labourOnQuoteModel->setData(labIndex, tmpRow.value(DBaseCtrl::Labour_Desc));

    labIndex = labourOnQuoteModel->index(labourSelection->currentIndex().row(), DBaseCtrl::LoQ_Hours);
    labourOnQuoteModel->setData(labIndex, 1);

    labIndex = labourOnQuoteModel->index(labourSelection->currentIndex().row(), DBaseCtrl::LoQ_Rate);
    labourOnQuoteModel->setData(labIndex, tmpRow.value(DBaseCtrl::Labour_Rate));

    QModelIndex tmp = labourOnQuoteModel->index(labourSelection->currentIndex().row(), DBaseCtrl::LoQ_VTotal);
    labourSelection->setCurrentIndex(tmp, QItemSelectionModel::Select);
    QKeyEvent *enterEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    viewKeyPressedLabour(enterEvent);

    labourSelection->setCurrentIndex(labourOnQuoteModel->index(
            labourSelection->currentIndex().row(), DBaseCtrl::LoQ_Name),
                                     QItemSelectionModel::Select);
    sumTotals();
}

void QuoteWidget::deleteLabour()
{
    if(labourSelection->currentIndex().row() == labourOnQuoteModel->rowCount() - 1)
        return;

    if(labourOnQuoteModel->rowCount() > 1)
    {
        labourOnQuoteModel->removeRow(labourSelection->currentIndex().row());
        labourOnQuoteModel->insertTotalCol();
        labourOnQuoteModel->submit();
        labourOnQuoteModel->insertRow(labourOnQuoteModel->rowCount() - 1);
    }
    resizeLabour();
    sumTotals();
}

void QuoteWidget::on_dateEdit_dateChanged(QDate )
{
    dateChanged = true;
    updateContact();
}
