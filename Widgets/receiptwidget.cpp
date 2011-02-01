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

#include "receiptwidget.h"
#include "ui_receiptwidget.h"
#include "dbasectrl.h"

#include <QSqlRecord>
#include <QSqlTableModel>
#include <QDebug>

ReceiptWidget::ReceiptWidget(int receiptID, DBaseCtrl *dbaseCtrl, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReceiptWidget)
{
    ui->setupUi(this);
    this->receiptID = receiptID;

    QSqlTableModel *receiptModel = dbaseCtrl->getReceiptModel();

    QSqlRecord record;
    for(int i = 0; i < receiptModel->rowCount(); i++)
    {
        record = receiptModel->record(i);
        if(record.value(DBaseCtrl::Receipt_ID).toInt() == receiptID)
            break;
    }

    QImage logo = dbaseCtrl->getLogo();
    ui->receiptBrowser->document()->addResource(QTextDocument::ImageResource, QUrl("logo://logo"), QVariant(logo));
    ui->receiptBrowser->setHtml(record.value(DBaseCtrl::Receipt_Details).toString());
}

ReceiptWidget::~ReceiptWidget()
{
    delete ui;
}
