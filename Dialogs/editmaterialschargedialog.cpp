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

#include "editmaterialschargedialog.h"
#include "ui_editmaterialschargedialog.h"

EditMaterialsChargeDialog::EditMaterialsChargeDialog(DBaseCtrl *dbCtrl, QModelIndex index, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditMaterialsChargeDialog)
{
    this->model = dbCtrl->getMaterialsModel();
    this->index = index;
    this->record = model->record(index.row());

    ui->setupUi(this);

    ui->nameEdit->setText(record.value("name").toString());
    ui->brandEdit->setText(record.value("brand").toString());
    ui->partNumEdit->setText(record.value("partNum").toString());
    ui->qtyEdit->setText(record.value("quantity").toString());
    ui->sellPriceEdit->setText(record.value("price").toString());
    ui->buyPriceEdit->setText(record.value("wholesalePrice").toString());
    ui->descriptionEdit->insertPlainText(record.value("description").toString());

    QRegExp numRegex("([0-9]*\\.[0-9][0-9])");

    ui->qtyEdit->setValidator(new QRegExpValidator(numRegex, this));
    ui->sellPriceEdit->setValidator(new QRegExpValidator(numRegex, this));
    ui->buyPriceEdit->setValidator(new QRegExpValidator(numRegex, this));
}

EditMaterialsChargeDialog::~EditMaterialsChargeDialog()
{
    delete ui;
}

void EditMaterialsChargeDialog::on_buttonBox_accepted()
{
    QSqlField name = record.field("name");
    QSqlField brand = record.field("brand");
    QSqlField partNum = record.field("partNum");
    QSqlField quantity = record.field("quantity");
    QSqlField price = record.field("price");
    QSqlField wholesalePrice = record.field("wholesalePrice");
    QSqlField description = record.field("description");

    name.setValue(ui->nameEdit->text());
    brand.setValue(ui->brandEdit->text());
    partNum.setValue(ui->partNumEdit->text());
    quantity.setValue(ui->qtyEdit->text());
    price.setValue(ui->sellPriceEdit->text());
    wholesalePrice.setValue(ui->buyPriceEdit->text());
    description.setValue(ui->descriptionEdit->toPlainText());

    record.replace(1, name);
    record.replace(2, brand);
    record.replace(3, description);
    record.replace(4, price);
    record.replace(5, quantity);
    record.replace(6, partNum);
    record.replace(7, wholesalePrice);


    model->setRecord(index.row(), record);
    model->submitAll();
}
