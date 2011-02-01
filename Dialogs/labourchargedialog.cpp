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

#include "labourchargedialog.h"
#include "ui_labourchargedialog.h"

#include "editlabourchargedialog.h"

#include <QInputDialog>
#include <QSqlTableModel>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>

#define NAME_COL 1
#define NAME_COL_WIDTH 150
#define DESC_COL 2
#define DESC_COL_WIDTH 260

LabourChargeDialog::LabourChargeDialog(DBaseCtrl* dbCtrl, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LabourChargeDialog)
{
    ui->setupUi(this);

    this->dbCtrl = dbCtrl;

    setWindowTitle("Labour");

    this->labourModel = dbCtrl->getLabourModel();

    labourProxyModel = new QSortFilterProxyModel(this);
    labourProxyModel->setSourceModel(labourModel);
    labourProxyModel->setDynamicSortFilter(true);

    ui->labourTable->setModel(labourProxyModel);
    labourSelectionModel = ui->labourTable->selectionModel();

    ui->labourTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->labourTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->labourTable->resizeColumnsToContents();
    ui->labourTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->labourTable->verticalHeader()->hide();
    ui->labourTable->horizontalHeader()->setStretchLastSection(true);

    ui->labourTable->setFocus();
    ui->labourTable->hideColumn(0);

    ui->labourTable->setColumnWidth(NAME_COL, NAME_COL_WIDTH);
    ui->labourTable->setColumnWidth(DESC_COL, DESC_COL_WIDTH);

    labourProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    labourProxyModel->sort(1);
    labourModel->select();

    ui->labourTable->selectRow(0);
    currColumn = 1;

    connect(ui->labourTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sortByColumn(int)));
}

LabourChargeDialog::~LabourChargeDialog()
{
    delete ui;
}

void LabourChargeDialog::on_editButton_clicked()
{
    if(labourProxyModel->rowCount() > 0)
    {
        QModelIndex currRow = labourSelectionModel->currentIndex();
        QModelIndex tmp = labourProxyModel->mapToSource(currRow);

        EditLabourChargeDialog chargeDialog(dbCtrl, tmp, this);
        chargeDialog.exec();

        QModelIndex last = labourProxyModel->mapFromSource(tmp);
        ui->labourTable->selectRow(last.row());
    }
}

void LabourChargeDialog::on_addButton_clicked()
{
    bool ok;
    QString newLabourName = QInputDialog::getText(this, "New Labour Charge",
                                                  "Enter the Name of the Labour type",
                                                  QLineEdit::Normal, "", &ok);

    if(ok && (newLabourName.length() > 1))
    {
        int newRow = labourModel->rowCount();
        labourModel->insertRow(newRow);
        labourModel->setData(labourModel->index(newRow, 1), newLabourName);
        labourModel->submit();

        ui->labourTable->selectRow(labourProxyModel->mapFromSource(labourModel->index(newRow, 1)).row());

        on_editButton_clicked();
    }
}


void LabourChargeDialog::on_lineEdit_textEdited(QString newText)
{
    QModelIndex firstIndex = ui->labourTable->currentIndex();
    QModelIndex finalIndex = labourProxyModel->mapToSource(firstIndex);

    int col = ui->comboBox->currentIndex() + 1;
    QString sortString = QString("^%1.*").arg(newText);

    labourProxyModel->setFilterRegExp(QRegExp(sortString, Qt::CaseInsensitive));
    labourProxyModel->setFilterKeyColumn(col);
    labourProxyModel->sort(1, Qt::AscendingOrder);
    sortByColumn(currColumn);

    int newIndex = labourProxyModel->mapFromSource(finalIndex).row();

    if(newIndex >= 0)
        ui->labourTable->selectRow(newIndex);
    else
        ui->labourTable->selectRow(0);

}

void LabourChargeDialog::on_comboBox_currentIndexChanged(QString )
{
    ui->lineEdit->setFocus();
    on_lineEdit_textEdited(ui->lineEdit->text());
}

void LabourChargeDialog::on_labourTable_doubleClicked(QModelIndex)
{
    on_editButton_clicked();
}

void LabourChargeDialog::sortByColumn(int col)
{
    QModelIndex tmpIndex = ui->labourTable->currentIndex();
    QModelIndex otherIndex = labourProxyModel->mapToSource(tmpIndex);
    currColumn = col;

    labourProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    labourProxyModel->sort(col);
    labourModel->select();

    ui->labourTable->selectRow(labourProxyModel->mapFromSource(otherIndex).row());
}

void LabourChargeDialog::on_deleteButton_clicked()
{
    QModelIndex currRow = labourProxyModel->mapToSource(labourSelectionModel->currentIndex());

    labourModel->removeRow(currRow.row());
    labourModel->submitAll();
}
