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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "firstrunwiz.h"
#include "imgobject.h"
#include "navpanel.h"
#include "contactdialog.h"
#include "addresschooserdialog.h"
#include "printcomposer.h"

#include <QTimeLine>
#include <QMessageBox>

/**
  *
  */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/Splash"));
    splash->show();
    bottomLeft = Qt::AlignBottom | Qt::AlignLeft;
    splash->showMessage(QObject::tr("Initialising ..."), bottomLeft, Qt::white);

    appSettings = new AppSettings();
    dbaseCtrl = new DBaseCtrl(appSettings);

    initChecks();
    splash->showMessage(QObject::tr("Gui Setup .."), bottomLeft, Qt::white);

    ui->setupUi(this);

    ui->navPanel->setDelegate(this);

    this->setMinimumHeight(650);
    this->setMinimumWidth(940);

    restoreGeometry(appSettings->getGeometry());

    paperDrawers = new ImgObject*[5];
    drawerFronts = new ImgObject*[5];

    dropMenus = new ImgObject**[5];
    dropMenus[quotes] = new ImgObject*[QUOTES_MENU_NUM];
    dropMenus[invoices] = new ImgObject*[INVOICES_MENU_NUM];
    dropMenus[receipts] = new ImgObject*[RECEIPTS_MENU_NUM];
    dropMenus[expenses] = new ImgObject*[EXPENSES_MENU_NUM];
    dropMenus[reports] = new ImgObject*[REPORTS_MENU_NUM];

    paperStates = new QState*[6];
    paperTrans = new QAbstractTransition*[11];
    drawStates = new QState*[5];
    drawerTrans = new QAbstractTransition*[9];
    menuStates = new QState**[5];
    menuTrans = new QAbstractTransition**[5];

    initMenu();
    initObjects();
    initStack();
    currentMenuState = homePosition;

    ui->listWidget->setStyleSheet("background-color : " + this->palette().window().color().name());

    //Setup the windows title and icon
    setWindowIcon(QPixmap(":/Logo"));
    setWindowTitle("TradieTrakka");

    //create new HomeWidget and add it to the the stack
    ui->stackedWidget->removeWidget(ui->stackedWidget->currentWidget());
    home = new HomeWidget(dbaseCtrl->getDbase());
    ui->stackedWidget->addWidget(home);
    //connect from home widget
    connect(home, SIGNAL(businessButtonClicked(ContactDialog::contactType)), this, SLOT(findContacts(ContactDialog::contactType)));
    connect(home, SIGNAL(personalButtonClicked(ContactDialog::contactType)), this, SLOT(findContacts(ContactDialog::contactType)));
    connect(home, SIGNAL(materialsButtonClicked()), this, SLOT(materialsEdit()));
    connect(home, SIGNAL(labourButtonClicked()), this, SLOT(labourEdit()));

    calcProcess = new QProcess(this);

    // init table models
    dbaseCtrl->initDBModels();
    splash->finish(this);
    delete splash;

    updateOverDueInvoice();
}

/**
  *
  */
void MainWindow::initChecks()
{
    if(appSettings->hasDbaseSettings())
        dbaseCtrl->connect();

    if(!dbaseCtrl->isOpen())
    {
        FirstRunWiz wiz(appSettings, dbaseCtrl);
        wiz.setMinimumWidth(650);

        splash->hide();
        wiz.exec();
        splash->show();
    }

    if(!dbaseCtrl->isOpen())
    {
        qWarning() << "Cannot connect to Database, now exiting";

        QMessageBox::critical(0,"Database Connect",
                       "Could not connect to Database!\n"
                       "Please check your settings");
        exit(0);
    }
}

/**
  *
  */
MainWindow::~MainWindow()
{
    delete dbaseCtrl;

    for(int i = 0; i < 5; ++i)
    {
        if(!paperDrawers[i])
            delete paperDrawers[i];
    }
    delete ui;
}

/**
  *
  */
void MainWindow::closeEvent(QCloseEvent *event)
 {
     appSettings->setGeometry(saveGeometry());
     QWidget::closeEvent(event);
 }

/**
  *
  */
/**
  *
  */
void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        //ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/**
  * Connects for menus to their relevant slots
  */
void MainWindow::initMenu()
{
    //Connect for New Quote
    connect(ui->actionQuote, SIGNAL(triggered()), this, SLOT(createNewQuote()));

    //Connect for New Invoice
    connect(ui->actionInvoice, SIGNAL(triggered()), this, SLOT(createNewInvoice()));

    //connect calc menu button to slot
    connect(ui->actionCalculator, SIGNAL(triggered()), this, SLOT(launchCalc()));

    //connect reports menu to there slots
    connect(ui->actionBASReport, SIGNAL(triggered()), this, SLOT(createBAS()));
    connect(ui->actionProfit_LossReport, SIGNAL(triggered()), this, SLOT(createPnL()));
    connect(ui->actionBreakdownReport, SIGNAL(triggered()), this, SLOT(createBreakdown()));

    //connect expences menu to there slots
    connect(ui->actionExpense, SIGNAL(triggered()), this, SLOT(createNewExpense()));

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), this,SLOT(aboutQtDialog()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutDialog()));
    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(on_closeButton_clicked()));

    //Connects for contacts
    connect(ui->actionCompanyContact, SIGNAL(triggered()), this, SLOT(addCompanyContact()));
    connect(ui->actionPersonalContact, SIGNAL(triggered()), this, SLOT(addPersonalContact()));
    //connect(ui->actionFind_Contact, SIGNAL(triggered()), this, SLOT(findContacts(CompanyDialog::All)));
    connect(ui->actionCompanyFind, SIGNAL(triggered()), this, SLOT(findContacts()));
    connect(ui->actionPersonalFind, SIGNAL(triggered()), this, SLOT(findContacts()));

    //Connects for settings dialog
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(editSettings()));



}

/**
  *
  */
void MainWindow::initStack()
{
    ui->listWidget->addItem("Home");
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(updateMenu()));
    ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
}

/**
  *
  */
void MainWindow::aboutDialog()
{
    QMessageBox aboutBox;
    aboutBox.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    aboutBox.about(this,

    "TradieTrakka",
    "<h3>About TradieTrakka</h3><br />"
    "TradieTrakka 0.6"
    "<p>TradieTrakka is an application for tradesmen to quickly and<br />"
    "easily manage their business administration.<br />"
    "TradieTrakka can look after Contacts, Quotes, Invoices and<br />"
    "Expenses.</p>"
    "<p><h4>Developers</h4>"
    "&nbsp;&nbsp;&nbsp;&nbsp;Steven Tucker and Craig Hume</p>"
    "<p><h4>Artwork</h4>"
    "&nbsp;&nbsp;&nbsp;&nbsp;Darcelle Hinze</p>"
    "<p>TradieTrakka is licensed under the GNU General Public License<br />"
    "version 3.0 <a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">GNU General Public License</a></p>"

    );
}

/**
  *
  */
void MainWindow::editSettings()
{
    SettingsDialog settingsDialog(dbaseCtrl);
    settingsDialog.exec();
}

/**
  *
  */
void MainWindow::addPersonalContact()
{


//    AddContactDialog contactDialog(dbaseCtrl->getPersonModel(), this);
//    contactDialog.exec();
}

/**
  *
  */
void MainWindow::addCompanyContact()
{
//    CompanyDialog companyDialog(dbaseCtrl, this);
//    companyDialog.exec();
}

/**
  *
  */
void MainWindow::findContacts(ContactDialog::contactType type)
{
    // add type to the function
    if(type == ContactDialog::Business)
    {
        ContactDialog contactDialog(type, dbaseCtrl, this);
        contactDialog.exec();

    }
    else
    {
        ContactDialog personalDialog(ContactDialog::Personal, dbaseCtrl, this);
        personalDialog.exec();
    }
}

void MainWindow::findContacts()
{
    if(QObject::sender()->objectName() == "actionCompanyFind")
        findContacts(ContactDialog::Business);
    else
        findContacts(ContactDialog::Personal);
}

/**
  *
  */
void MainWindow::materialsEdit()
{
    MaterialPricesDialog materialsDialog(dbaseCtrl, this);
    materialsDialog.exec();
}

/**
  *
  */
void MainWindow::labourEdit()
{
    LabourChargeDialog labourDialog(dbaseCtrl, this);
    labourDialog.exec();
}

/**
  *
  */
void MainWindow::aboutQtDialog()
{
    QMessageBox aboutQtBox;
    aboutQtBox.aboutQt(this);
}

/**
  *
  */
void MainWindow::createNewQuote()
{
    emit quotePaperPos();
    emit paperFromQuote();
}

/**
  *
  */
void MainWindow::openNewQuote()
{
    bool ok;

    QStringList choices;
    choices << "Company" << "Person";

    QString answer = QInputDialog::getItem(this, "Choose Contact Type", "Select the type of contact for this quote",
                                           choices, 0, false, &ok);

    int id;

    if(ok && !answer.isEmpty())
    {
        int entityType;
        int contactId;

        // Launch dialog for contact details
        if(answer == "Company")
        {
            entityType = 0;
            ContactDialog cd(ContactDialog::Business, dbaseCtrl, this);
            contactId = cd.exec();
        } else {
            entityType = 1;
            ContactDialog cd(ContactDialog::Personal, dbaseCtrl, this);
            contactId = cd.exec();
        }
        // Check for multiple and retrive address
        QSqlQueryModel *contactAddressModel;

        if(entityType == 0)
            contactAddressModel = dbaseCtrl->getCompanyAddressesModel(contactId);
        else
            contactAddressModel = dbaseCtrl->getPersonAddressModel(contactId);

        int addressID;

        if(contactAddressModel->rowCount() > 1)
        {
            if(entityType == 0)
            {
                AddressChooserDialog ac(dbaseCtrl, AddressChooserDialog::Business, contactId, this);
                addressID = ac.exec();
            }
            else
            {
                AddressChooserDialog ac(dbaseCtrl, AddressChooserDialog::Personal, contactId, this);
                addressID = ac.exec();
            }
        }
        else
        {
            addressID = contactAddressModel->record(0).value("id").toInt();
        }

        // Create new Quote
        QSqlTableModel *tmp = dbaseCtrl->getQuoteModel();
        tmp->insertRow(tmp->rowCount());
        tmp->setData(tmp->index(tmp->rowCount() - 1, DBaseCtrl::Quote_QDate),  QDate::currentDate().toString(DATE_FORMAT));
        tmp->setData(tmp->index(tmp->rowCount() - 1, DBaseCtrl::Quote_EntityType),  entityType);
        tmp->setData(tmp->index(tmp->rowCount() - 1, DBaseCtrl::Quote_EntityID), contactId);
        tmp->setData(tmp->index(tmp->rowCount() - 1, DBaseCtrl::Quote_Address), addressID);

        tmp->submit();
        id = tmp->record(tmp->rowCount() - 1).value(DBaseCtrl::Quote_ID).toInt();

    } else {
        qDebug("Dialog to retrieve contact type failed");
        return;
    }

    QuoteWidget* tmpWidget = new QuoteWidget(dbaseCtrl, id);
    ui->stackedWidget->addWidget(tmpWidget);

    QString qName = "Quote " + QString::number(id) + " " + tmpWidget->getContactName();
    ui->listWidget->addItem(qName);
    ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);

    emit homePos();
}

/**
  *
  */
void MainWindow::openExistingQuote()
{
    FindDialog quoteFindDialog(FindDialog::Quote, dbaseCtrl, this);
    int id = quoteFindDialog.exec();

    if(id != -1)
    {
        QuoteWidget* tmpWidget = new QuoteWidget(dbaseCtrl, id);
        ui->stackedWidget->addWidget(tmpWidget);
        ui->listWidget->addItem("Quote " + QString::number(id) + " " + tmpWidget->getContactName());
        ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
    }
}

/**
  *
  */
void MainWindow::quoteToInvoice()
{
    QuoteWidget *quoteWidget = qobject_cast<QuoteWidget*>(ui->stackedWidget->currentWidget());
    if(quoteWidget)
    {
        int quoteID, invoiceRow, invoiceID;

        QSqlTableModel *quoteModel = dbaseCtrl->getQuoteModel();
        QSqlTableModel *invoiceModel = dbaseCtrl->getInvoiceModel();
        QSqlRecord quoteRecord;

        invoiceModel->insertRow(invoiceModel->rowCount());

        invoiceRow = invoiceModel->rowCount() -1;

        quoteID = quoteWidget->getQuoteId();

        // Find the Quote Record that has the quoteID
        QModelIndex quoteRow;
        for(int row = 0; row < quoteModel->rowCount(); row++)
        {
            quoteRow = quoteModel->index(row, 0);
            quoteRecord = quoteModel->record(quoteRow.row());
            if(quoteRecord.value(DBaseCtrl::Quote_ID).toInt() == quoteID)
                break;
        }

        invoiceModel->setData(invoiceModel->index(invoiceRow, DBaseCtrl::Invoice_IDate),
                              QDate::currentDate().toString(DATE_FORMAT));

        invoiceModel->setData(invoiceModel->index(invoiceRow, DBaseCtrl::Invoice_DueDate),
                              QDate::currentDate().addMonths(1).toString(DATE_FORMAT));

        invoiceModel->setData(invoiceModel->index(invoiceRow, DBaseCtrl::Invoice_PaidDate),
                              QDate::currentDate().toString(DATE_FORMAT));

        // Copy over the fields for the invoice

        invoiceModel->setData(invoiceModel->index(invoiceRow, DBaseCtrl::Invoice_EntityType),
                              quoteRecord.value(DBaseCtrl::Quote_EntityType));

        invoiceModel->setData(invoiceModel->index(invoiceRow, DBaseCtrl::Invoice_EntityId),
                              quoteRecord.value(DBaseCtrl::Quote_EntityID));

        invoiceModel->setData(invoiceModel->index(invoiceRow, DBaseCtrl::Invoice_Address),
                              quoteRecord.value(DBaseCtrl::Quote_Address));

        invoiceModel->setData(invoiceModel->index(invoiceRow, DBaseCtrl::Invoice_Details),
                              quoteRecord.value(DBaseCtrl::Quote_Details));

        invoiceModel->setData(invoiceModel->index(invoiceRow, DBaseCtrl::Invoice_Status),
                              InvoiceWidget::Awaiting);

        invoiceModel->submit();

        invoiceID = invoiceModel->record(invoiceRow).value(DBaseCtrl::Invoice_ID).toInt();

        // Copy over Materials for the invoice
        int invoiceMatIndex;
        QModelIndex quoteMatRow;
        QSqlRecord quoteMatRecord;

        CSqlTableModel *quoteMatModel = dbaseCtrl->getMaterialsOnQuoteModel(quoteID);
        CSqlTableModel *invoiceMatModel = dbaseCtrl->getMaterialsOnInvoiceModel(invoiceID);
        for(int row = 0; row < quoteMatModel->rowCount(); row++)
        {
            quoteMatRow = quoteMatModel->index(row, 0);
            quoteMatRecord = quoteMatModel->record(quoteMatRow.row());

            invoiceMatIndex = invoiceMatModel->rowCount();
            invoiceMatModel->insertRow(invoiceMatIndex);

            invoiceMatModel->setData(invoiceMatModel->index(invoiceMatIndex, DBaseCtrl::MoI_Name),
                                     quoteMatRecord.value(DBaseCtrl::MoQ_Name));

            invoiceMatModel->setData(invoiceMatModel->index(invoiceMatIndex, DBaseCtrl::MoI_Brand),
                                     quoteMatRecord.value(DBaseCtrl::MoQ_Brand));

            invoiceMatModel->setData(invoiceMatModel->index(invoiceMatIndex, DBaseCtrl::MoI_Price),
                                     quoteMatRecord.value(DBaseCtrl::MoQ_Price));

            invoiceMatModel->setData(invoiceMatModel->index(invoiceMatIndex, DBaseCtrl::MoI_QTY),
                                     quoteMatRecord.value(DBaseCtrl::MoQ_QTY));

            invoiceMatModel->setData(invoiceMatModel->index(invoiceMatIndex, DBaseCtrl::MoI_InvoiceID),
                                     invoiceID);

            invoiceMatModel->submit();
        }

        // Copy over the Labour for the invoice
        int invoiceLabIndex;
        QModelIndex quoteLabRow;
        QSqlRecord quoteLabRecord;

        CSqlTableModel *quoteLabModel = dbaseCtrl->getLabourOnQuoteModel(quoteID);
        CSqlTableModel *invoiceLabModel = dbaseCtrl->getLabourOnInvoiceModel(invoiceID);
        for(int row = 0; row < quoteLabModel->rowCount(); row++)
        {
            quoteLabRow = quoteLabModel->index(row, 0);
            quoteLabRecord = quoteLabModel->record(quoteLabRow.row());

            invoiceLabIndex = invoiceLabModel->rowCount();
            invoiceLabModel->insertRow(invoiceLabIndex);

            invoiceLabModel->setData(invoiceLabModel->index(invoiceLabIndex, DBaseCtrl::LoI_Name),
                                     quoteLabRecord.value(DBaseCtrl::LoQ_Name));

            invoiceLabModel->setData(invoiceLabModel->index(invoiceLabIndex, DBaseCtrl::LoI_Rate),
                                     quoteLabRecord.value(DBaseCtrl::LoQ_Rate));

            invoiceLabModel->setData(invoiceLabModel->index(invoiceLabIndex, DBaseCtrl::LoI_Hours),
                                     quoteLabRecord.value(DBaseCtrl::LoQ_Hours));

            invoiceLabModel->setData(invoiceLabModel->index(invoiceLabIndex, DBaseCtrl::LoI_Description),
                                     quoteLabRecord.value(DBaseCtrl::LoQ_Description));

            invoiceLabModel->setData(invoiceLabModel->index(invoiceLabIndex, DBaseCtrl::LoI_InvoiceID),
                                     invoiceID);

            invoiceLabModel->submit();
        }

        //invoiceModel->submit();

        InvoiceWidget* tmpWidget = new InvoiceWidget(dbaseCtrl, invoiceID);
        QString invoiceTitle = "Invoice " + QString::number(invoiceID) + " " + tmpWidget->getContactName();
        ui->stackedWidget->addWidget(tmpWidget);
        ui->listWidget->addItem(invoiceTitle);
        ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);

        connect(tmpWidget, SIGNAL(updateOverDueNotice()), this, SLOT(updateOverDueInvoice()));
    }
}

/**
  *
  */
void MainWindow::quoteDelete()
{
    QuoteWidget *quoteWidget = qobject_cast<QuoteWidget*>(ui->stackedWidget->currentWidget());
    if(quoteWidget)
    {
        QMessageBox msgBox;
        msgBox.setText("Are you sure you want to Delete this Quote?");
        msgBox.setInformativeText("Once deleted you cannot retrieve this Quote!");
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();

        if(ret == QMessageBox::Ok)
        {

            bool ok = dbaseCtrl->removeQuote(quoteWidget->getQuoteId());
            if(!ok)
                QMessageBox::warning(this, "Remove Quote Error", "Could not remove Quote!!");
            else
                removeCurrentItem(ui->stackedWidget->currentIndex());

        }
    }
}

/**
  *
  */
void MainWindow::createNewInvoice()
{
    opening = true;
    emit openInvoiceDraw();
}

/**
  *
  */
void MainWindow::invoiceDrawerClosed()
{
    if(opening)
    {
        emit invoicePaperPos();
        emit paperFromInvoice();
    } else {
        emit paperToInvoice();
    }
    opening = true;
}

/**
  *
  */
void MainWindow::openNewInvoice()
{
    bool ok;

    QStringList choices;
    choices << "Company" << "Person";

    QString answer = QInputDialog::getItem(this, "Choose Contact Type", "Select the type of contact for this quote",
                                           choices, 0, false, &ok);

    int id;

    if(ok && !answer.isEmpty())
    {
        int entityType;
        int contactId;

        // Launch dialog for contact details
        if(answer == "Company")
        {
            entityType = 0;
            ContactDialog cd(ContactDialog::Business, dbaseCtrl, this);
            contactId = cd.exec();
        } else {
            entityType = 1;
            ContactDialog cd(ContactDialog::Personal, dbaseCtrl, this);
            contactId = cd.exec();
        }
        // Check for multiple and retrive address
        QSqlQueryModel *contactAddressModel;

        if(entityType == 0)
            contactAddressModel = dbaseCtrl->getCompanyAddressesModel(contactId);
        else
            contactAddressModel = dbaseCtrl->getPersonAddressModel(contactId);

        int addressID;

        if(contactAddressModel->rowCount() > 1)
        {
            if(entityType == 0)
            {
                AddressChooserDialog ac(dbaseCtrl, AddressChooserDialog::Business, contactId, this);
                addressID = ac.exec();
            }
            else
            {
                AddressChooserDialog ac(dbaseCtrl, AddressChooserDialog::Personal, contactId, this);
                addressID = ac.exec();
            }
        }
        else
        {
            addressID = contactAddressModel->record(0).value("id").toInt();
        }

        // Create new Invoice
        QSqlTableModel *tmp = dbaseCtrl->getInvoiceModel();
        tmp->insertRow(tmp->rowCount());
        tmp->setData(tmp->index(tmp->rowCount() - 1, DBaseCtrl::Invoice_IDate),  QVariant(QDate::currentDate().toString(DATE_FORMAT)));
        tmp->setData(tmp->index(tmp->rowCount() - 1, DBaseCtrl::Invoice_DueDate), QVariant(QDate::currentDate().addMonths(1).toString(DATE_FORMAT)));
        tmp->setData(tmp->index(tmp->rowCount() - 1, DBaseCtrl::Invoice_PaidDate),  QVariant(QDate::currentDate().toString(DATE_FORMAT)));
        tmp->setData(tmp->index(tmp->rowCount() - 1, DBaseCtrl::Invoice_EntityType),  entityType);
        tmp->setData(tmp->index(tmp->rowCount() - 1, DBaseCtrl::Invoice_EntityId), contactId);
        tmp->setData(tmp->index(tmp->rowCount() - 1, DBaseCtrl::Invoice_Address), addressID);
        tmp->setData(tmp->index(tmp->rowCount() - 1, DBaseCtrl::Invoice_Status), InvoiceWidget::Awaiting);

        tmp->submit();
        id = tmp->record(tmp->rowCount() - 1).value(DBaseCtrl::Invoice_ID).toInt();

        InvoiceWidget *tmpWidget = new InvoiceWidget(dbaseCtrl, id);
        QString iName = "Invoice " + QString::number(id) + " " + tmpWidget->getContactName();
        ui->stackedWidget->addWidget(tmpWidget);
        ui->listWidget->addItem(iName);
        ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);

        connect(tmpWidget, SIGNAL(updateOverDueNotice()), this, SLOT(updateOverDueInvoice()));
    } else {
        emit homePos();
    }
}

/**
  *
  */
void MainWindow::openExistingInvoice()
{
    FindDialog invoiceFindDialog(FindDialog::Invoice, dbaseCtrl, this);
    int id = invoiceFindDialog.exec();

    if(id != -1)
    {
        InvoiceWidget *tmpWidget = new InvoiceWidget(dbaseCtrl, id);
        ui->stackedWidget->addWidget(tmpWidget);
        ui->listWidget->addItem("Invoice " + QString::number(id) + " " + tmpWidget->getContactName());
        ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);

        InvoiceWidget *tmp = qobject_cast<InvoiceWidget*>(ui->stackedWidget->currentWidget());
        tmp->lockInvoice();

        connect(tmpWidget, SIGNAL(updateOverDueNotice()), this, SLOT(updateOverDueInvoice()));
    }
}

/**
  *
  */
void MainWindow::invoiceToReceipt()
{
    InvoiceWidget *invoice = qobject_cast<InvoiceWidget*>(ui->stackedWidget->currentWidget());
    if(invoice)
    {
        if(invoice->getStatus() == InvoiceWidget::Paid)
        {
            QSqlTableModel *receiptModel = dbaseCtrl->getReceiptModel();

            int receiptRow = receiptModel->rowCount();
            receiptModel->insertRow(receiptRow);

            receiptModel->setData(receiptModel->index(receiptRow, DBaseCtrl::Receipt_InvoiceID),
                                  invoice->getInvoiceID());

            receiptModel->setData(receiptModel->index(receiptRow, DBaseCtrl::Receipt_RDate),
                                  QDate::currentDate().toString(DATE_FORMAT));

            receiptModel->submit();

            // Generate the Receipt Display
            int receiptNum = receiptModel->data(receiptModel->index(receiptRow, DBaseCtrl::Receipt_ID)).toInt();
            QString display = PrintComposer::receiptString(receiptNum, dbaseCtrl);

            receiptModel->setData(receiptModel->index(receiptRow, DBaseCtrl::Receipt_Details), display);

            receiptModel->submit();

            // Find and pass the receiptID
            int receiptID = receiptModel->data(
                    receiptModel->index(receiptRow, DBaseCtrl::Receipt_ID)).toInt();

            ui->stackedWidget->addWidget(new ReceiptWidget(receiptID, dbaseCtrl));

            ui->listWidget->addItem("Receipt    " + QString::number(receiptID));
            ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
        } else {
            QMessageBox notPaid;
            notPaid.setText("The Invoice has not been paid, so a receipt cannot be generated");
            notPaid.setIcon(QMessageBox::Warning);
            notPaid.exec();
        }
    }
}

/**
  *
  */
void MainWindow::invoicesOverdue()
{
    FindDialog invoiceFindDialog(FindDialog::OverDue, dbaseCtrl, this);
    int id = invoiceFindDialog.exec();

    if(id != -1)
    {
        InvoiceWidget *tmpWidget = new InvoiceWidget(dbaseCtrl, id);
        ui->stackedWidget->addWidget(tmpWidget);
        ui->listWidget->addItem("Invoice " + QString::number(id) + " " + tmpWidget->getContactName());
        ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);

        InvoiceWidget *tmp = qobject_cast<InvoiceWidget*>(ui->stackedWidget->currentWidget());
        tmp->lockInvoice();
    }
}

/**
  *
  */
void MainWindow::createNewReceipt()
{
    opening = true;
    emit openReceiptDraw();
}

/**
  *
  */
void MainWindow::receiptDrawerClosed()
{
    if(opening)
    {
        emit receiptPaperPos();
        emit paperFromReceipt();
    } else {
        emit paperToReceipt();
    }
    opening = true;
}

/**
  *
  */
void MainWindow::openNewReceipt()
{
//    ui->stackedWidget->addWidget(new ReceiptWidget);
//    ui->listWidget->addItem("New Receipt");
//    ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
}

/**
  *
  */
void MainWindow::openExistingReceipt()
{
    FindDialog receiptFindDialog(FindDialog::Receipt, dbaseCtrl, this);
    int id = receiptFindDialog.exec();

    if(id != -1)
    {
        ui->stackedWidget->addWidget(new ReceiptWidget(id, dbaseCtrl));
        ui->listWidget->addItem("Receipt   " + QString::number(id));
        ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
    }
}

/**
  *
  */
void MainWindow::createNewExpense()
{
    opening = true;
    emit openExpenseDraw();
}

/**
  *
  */
void MainWindow::expenseDrawerClosed()
{
    if(opening)
    {
        emit expensePaperPos();
        emit paperFromExpense();
    } else {
        emit paperToExpense();
        opening = true;
    }
}

/**
  *
  */
void MainWindow::openNewExpense()
{
    bool ok;
    QString expenseName = QInputDialog::getText(this, "Expense Name", "Enter the name of the new Expense",
                                                QLineEdit::Normal, "", &ok);

    if(ok && expenseName.length() > 1)
    {
        QSqlTableModel *expenseModel = dbaseCtrl->getExpenseModel();
        int row = expenseModel->rowCount();
        expenseModel->insertRow(row);
        expenseModel->setData(expenseModel->index(row, DBaseCtrl::Expense_Name), expenseName);
        expenseModel->setData(expenseModel->index(row, DBaseCtrl::Expense_EDate), QDate::currentDate().toString(DATE_FORMAT));
        expenseModel->setData(expenseModel->index(row, DBaseCtrl::Expense_Qty), "0");
        expenseModel->setData(expenseModel->index(row, DBaseCtrl::Expense_Amount), "0");
        expenseModel->setData(expenseModel->index(row, DBaseCtrl::Expense_GST), "0");
        expenseModel->setData(expenseModel->index(row, DBaseCtrl::Expense_Desc), " ");
        expenseModel->submitAll();

        QModelIndex expenseIndex = expenseModel->index(row, DBaseCtrl::Expense_ID);

        int id = expenseModel->data(expenseIndex).toInt();

        ui->stackedWidget->addWidget(new ExpensesWidget(dbaseCtrl, id));
        ui->listWidget->addItem("Expense - " + expenseName);
        ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
    } else {
        emit homePos();
    }
}

/**
  *
  */
void MainWindow::openExistingExpense()
{
    FindDialog expenseFindDialog(FindDialog::Expense, dbaseCtrl, this);
    int id = expenseFindDialog.exec();

    if(id != -1)
    {
        ui->stackedWidget->addWidget(new ExpensesWidget(dbaseCtrl, id));
        ui->listWidget->addItem("Expense   " + QString::number(id));
        ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
    }
}

/**
  *
  */
void MainWindow::createNewReport()
{
    opening = true;
    // Check the type of report requested and call the appropriate function
    createBAS();
}

/**
  *
  */
void MainWindow::reportDrawerClosed()
{
    if(opening)
    {
        emit reportPaperPos();
        emit paperFromReport();
    } else {
        emit paperToReport();
        opening = true;
    }
}

/**
  *
  */
void MainWindow::openNewReport()
{
    ui->stackedWidget->addWidget(new ReportingWidget(reportType, dbaseCtrl));
    ui->listWidget->addItem(reportLabel);
    ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
}

/**
  *
  */
void MainWindow::createBAS()
{
    reportType = ReportingWidget::Bas;
    reportLabel = "Bas Report";
    emit openReportDraw();
}

/**
  *
  */
void MainWindow::createPnL()
{
    reportType = ReportingWidget::PnL;
    reportLabel = "Profit and Loss Report";
    emit openReportDraw();
}

/**
  *
  */
void MainWindow::createBreakdown()
{
    reportType = ReportingWidget::Breakdown;
    reportLabel = "Breakdown Report";
    emit openReportDraw();
}

/**
  *
  */
void MainWindow::on_closeButton_clicked()
{
    ExpensesWidget *exWidg = qobject_cast<ExpensesWidget*>(ui->stackedWidget->currentWidget());
    if(exWidg)
        exWidg->submitData();

    opening = false;
    int index = ui->listWidget->currentIndex().row();
    if(index)
        removeCurrentItem(index);
}


/**
  *
  */
void MainWindow::removeCurrentItem(int index)
{
    QWidget *widget = ui->stackedWidget->widget(index);
    HomeWidget *homeWidget = qobject_cast<HomeWidget *>(widget);
    QuoteWidget *quoteWidget = qobject_cast<QuoteWidget *>(widget);
    ReceiptWidget *receiptWidget = qobject_cast<ReceiptWidget *>(widget);
    ExpensesWidget *expenseWidget = qobject_cast<ExpensesWidget *>(widget);
    InvoiceWidget *invoiceWidget = qobject_cast<InvoiceWidget *>(widget);
    ReportingWidget *reportingWidget = qobject_cast<ReportingWidget *>(widget);


    ui->listWidget->takeItem(index);
    ui->stackedWidget->removeWidget(ui->stackedWidget->widget(index));

    if(homeWidget)
        emit homePos();
    else if(quoteWidget)
        emit paperToQuote();
    else if(invoiceWidget)
        emit openInvoiceDraw();
    else if(receiptWidget)
        emit(openReceiptDraw());
    else if(expenseWidget)
        emit(openExpenseDraw());
    else if(reportingWidget)
        emit openReportDraw();
}

/**
  *
  */
void MainWindow::launchCalc()
{
    QString calcProgram;

    #ifdef Q_WS_X11
            calcProgram = "xcalc";
    #endif

    #ifdef Q_WS_WIN
            calcProgram = "calc.exe";
    #endif

    #ifdef Q_WS_MAC
            calcProgram = "/Applications/Calculator.app/Contents/MacOS/Calculator";
    #endif

    calcProcess->startDetached(calcProgram);
}

void MainWindow::on_pdfButton_clicked()
{
    int index = ui->stackedWidget->currentIndex();

    QWidget *widget = ui->stackedWidget->widget(index);
    HomeWidget *homeWidget = qobject_cast<HomeWidget *>(widget);
    QuoteWidget *quoteWidget = qobject_cast<QuoteWidget *>(widget);
    ReceiptWidget *receiptWidget = qobject_cast<ReceiptWidget *>(widget);
    ExpensesWidget *expenseWidget = qobject_cast<ExpensesWidget *>(widget);
    InvoiceWidget *invoiceWidget = qobject_cast<InvoiceWidget *>(widget);
    ReportingWidget *reportingWidget = qobject_cast<ReportingWidget *>(widget);

    if(homeWidget)
        return;
    else if(quoteWidget)
    {
        QMessageBox optionBox;
        optionBox.setText("Would you like a detailed Quote?");
        optionBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        optionBox.setDefaultButton(QMessageBox::Yes);

        if(optionBox.exec() == QMessageBox::Yes)
        {
            PrintComposer::printString(PrintComposer::quoteString(
                    quoteWidget->getQuoteId(), dbaseCtrl),
                    PrintComposer::PDF, dbaseCtrl->getLogo());
        }
        else
        {
            PrintComposer::printString(PrintComposer::quoteString(
                    quoteWidget->getQuoteId(), dbaseCtrl, false),
                    PrintComposer::PDF, dbaseCtrl->getLogo());
        }

    }
    else if(invoiceWidget)
    {
        QMessageBox optionBox;
        optionBox.setText("Would you like a detailed Invoice?");
        optionBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        optionBox.setDefaultButton(QMessageBox::Yes);

        if(optionBox.exec() == QMessageBox::Yes)
        {
            PrintComposer::printString(PrintComposer::invoiceString(
                    invoiceWidget->getInvoiceID(), dbaseCtrl),
                    PrintComposer::PDF, dbaseCtrl->getLogo());
        }
        else
        {
            PrintComposer::printString(PrintComposer::invoiceString(
                    invoiceWidget->getInvoiceID(), dbaseCtrl, false),
                    PrintComposer::PDF, dbaseCtrl->getLogo());
        }
    }
    else if(receiptWidget)
    {
        PrintComposer::printString(PrintComposer::receiptString(
                receiptWidget->getReceiptID(), dbaseCtrl),
                PrintComposer::PDF, dbaseCtrl->getLogo());
    }
    else if(expenseWidget)
    {
        expenseWidget->submitData();
        PrintComposer::printString(PrintComposer::expenseString(
                expenseWidget->getExpenseID(), dbaseCtrl),
                PrintComposer::PDF, dbaseCtrl->getLogo());
    }
    else if(reportingWidget)
    {
        PrintComposer::printString(reportingWidget->getHtml(),
                                   PrintComposer::PDF, dbaseCtrl->getLogo());
    }
}

void MainWindow::on_printButton_clicked()
{
    int index = ui->stackedWidget->currentIndex();

    QWidget *widget = ui->stackedWidget->widget(index);
    HomeWidget *homeWidget = qobject_cast<HomeWidget *>(widget);
    QuoteWidget *quoteWidget = qobject_cast<QuoteWidget *>(widget);
    ReceiptWidget *receiptWidget = qobject_cast<ReceiptWidget *>(widget);
    ExpensesWidget *expenseWidget = qobject_cast<ExpensesWidget *>(widget);
    InvoiceWidget *invoiceWidget = qobject_cast<InvoiceWidget *>(widget);
    ReportingWidget *reportingWidget = qobject_cast<ReportingWidget *>(widget);

    if(homeWidget)
        return;
    else if(quoteWidget)
    {
        QMessageBox optionBox;
        optionBox.setText("Would you like a detailed Quote?");
        optionBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        optionBox.setDefaultButton(QMessageBox::Yes);

        if(optionBox.exec() == QMessageBox::Yes)
        {
            PrintComposer::printString(PrintComposer::quoteString(
                    quoteWidget->getQuoteId(), dbaseCtrl),
                    PrintComposer::Print, dbaseCtrl->getLogo());
        }
        else
        {
            PrintComposer::printString(PrintComposer::quoteString(
                    quoteWidget->getQuoteId(), dbaseCtrl, false),
                    PrintComposer::Print, dbaseCtrl->getLogo());
        }
    }
    else if(invoiceWidget)
    {
        QMessageBox optionBox;
        optionBox.setText("Would you like a detailed Quote?");
        optionBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        optionBox.setDefaultButton(QMessageBox::Yes);

        if(optionBox.exec() == QMessageBox::Yes)
        {
            PrintComposer::printString(PrintComposer::invoiceString(
                    invoiceWidget->getInvoiceID(), dbaseCtrl),
                    PrintComposer::Print, dbaseCtrl->getLogo());
        }
        else
        {
            PrintComposer::printString(PrintComposer::invoiceString(
                    invoiceWidget->getInvoiceID(), dbaseCtrl, false),
                    PrintComposer::Print, dbaseCtrl->getLogo());
        }
    }
    else if(receiptWidget)
    {
        PrintComposer::printString(PrintComposer::receiptString(
                receiptWidget->getReceiptID(), dbaseCtrl),PrintComposer::Print, dbaseCtrl->getLogo());
    }
    else if(expenseWidget)
    {
        expenseWidget->submitData();
        PrintComposer::printString(PrintComposer::expenseString(
                expenseWidget->getExpenseID(), dbaseCtrl),PrintComposer::Print, dbaseCtrl->getLogo());
    }
    else if(reportingWidget)
    {
        PrintComposer::printString(reportingWidget->getHtml(),
                                   PrintComposer::Print, dbaseCtrl->getLogo());
    }
}

void MainWindow::updateOverDueInvoice()
{
    int overDueInvoiceNum = dbaseCtrl->getNumberOfOverDue();

    if(overDueInvoiceNum == 1)
    {
        ui->statusBar->showMessage("There is 1 overdue Invoice");
    } else {
        ui->statusBar->showMessage("There are " + QString::number(overDueInvoiceNum) + " overdue invoices");
    }
}
