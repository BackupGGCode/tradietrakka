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
