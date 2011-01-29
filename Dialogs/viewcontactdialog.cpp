#include "viewcontactdialog.h"
#include "ui_viewcontactdialog.h"

#include <QSqlQueryModel>
#include <QSortFilterProxyModel>

#include "printcomposer.h"

ViewContactDialog::ViewContactDialog(ViewType viewType, DBaseCtrl *dbCtrl, int entityID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewContactDialog)
{
    ui->setupUi(this);
    this->dbCtrl = dbCtrl;
    this->entityID = entityID;
    this->viewType = viewType;

    switch(viewType)
    {
    case Company:
        viewCompany();
        break;
    case Personal:
        viewPerson();
        break;
    }
}

void ViewContactDialog::viewPerson()
{
    ui->textBrowser->setHtml(PrintComposer::personalContact(entityID, dbCtrl));
}

void ViewContactDialog::viewCompany()
{
    ui->textBrowser->setHtml(PrintComposer::companyContact(entityID, dbCtrl));
}

ViewContactDialog::~ViewContactDialog()
{
    delete ui;
}

void ViewContactDialog::on_closeButton_clicked()
{
    close();
}

void ViewContactDialog::on_printButton_clicked()
{
    PrintComposer::printString(ui->textBrowser->toHtml(), PrintComposer::Print, QImage());
}

void ViewContactDialog::on_pdfButton_clicked()
{
    PrintComposer::printString(ui->textBrowser->toHtml(), PrintComposer::PDF, QImage());
}
