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

#include "editlabourchargedialog.h"
#include "ui_editlabourchargedialog.h"

#include <QSqlRecord>
#include <QSqlTableModel>
#include <QDataWidgetMapper>

EditLabourChargeDialog::EditLabourChargeDialog(DBaseCtrl *dbCtrl, QModelIndex labourIndex, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditLabourChargeDialog)
{
    ui->setupUi(this);

    setWindowTitle("Labour Rates");
    this->dbCtrl = dbCtrl;
    this->labourIndex = labourIndex;
    this->model = dbCtrl->getLabourModel();


    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setModel(this->model);

    mapper->addMapping(ui->nameEdit, Labour_name);
    mapper->addMapping(ui->descriptionEdit, Labour_desc);
    mapper->addMapping(ui->rateEdit, Labour_rate);

    mapper->setCurrentModelIndex(labourIndex);

    QRegExp numRegex("([0-9]*\\.[0-9][0-9])");

    ui->rateEdit->setValidator(new QRegExpValidator(numRegex, this));
}

EditLabourChargeDialog::~EditLabourChargeDialog()
{
    delete ui;
}

void EditLabourChargeDialog::on_buttonBox_accepted()
{
    mapper->submit();
}

void EditLabourChargeDialog::on_buttonBox_rejected()
{
    model->revertRow(mapper->currentIndex());
}
