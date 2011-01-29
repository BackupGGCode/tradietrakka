#include "materialpricesdialog.h"
#include "editmaterialschargedialog.h"
#include "ui_materialpricesdialog.h"

#include <QInputDialog>
#include <QSqlTableModel>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>

#define NAME_COL_WIDTH 100
#define BRAND_COL_WIDTH 100
#define DESC_COL_WIDTH 150
#define SELL_COL_WIDTH 80
#define QTY_COL_WIDTH 80
#define PARTNUM_COL_WIDTH 100
#define BUY_COL_WIDTH 80

MaterialPricesDialog::MaterialPricesDialog(DBaseCtrl* dbCtrl, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MaterialPricesDialog)
{
    ui->setupUi(this);

    this->dbCtrl = dbCtrl;

    setWindowTitle("Materials");

    this->materialModel = dbCtrl->getMaterialsModel();

    materialProxyModel = new QSortFilterProxyModel(this);
    materialProxyModel->setSourceModel(materialModel);
    materialProxyModel->setDynamicSortFilter(true);

    ui->materialsTable->setModel(materialProxyModel);
    materialselectionModel = ui->materialsTable->selectionModel();

    ui->materialsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->materialsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->materialsTable->resizeColumnsToContents();
    ui->materialsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->materialsTable->verticalHeader()->hide();
    ui->materialsTable->horizontalHeader()->setStretchLastSection(true);

    ui->materialsTable->setFocus();
    ui->materialsTable->hideColumn(0);
    ui->materialsTable->setSortingEnabled(true);

    materialProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    materialProxyModel->sort(1);

    materialProxyModel->setHeaderData(DBaseCtrl::Mat_Name, Qt::Horizontal, "Name");
    materialProxyModel->setHeaderData(DBaseCtrl::Mat_Brand, Qt::Horizontal, "Brand");
    materialProxyModel->setHeaderData(DBaseCtrl::Mat_Description, Qt::Horizontal, "Description");
    materialProxyModel->setHeaderData(DBaseCtrl::Mat_Price, Qt::Horizontal, "Sell Price");
    materialProxyModel->setHeaderData(DBaseCtrl::Mat_Quantity, Qt::Horizontal, "Quantity");
    materialProxyModel->setHeaderData(DBaseCtrl::Mat_PartNum, Qt::Horizontal, "Part Number");
    materialProxyModel->setHeaderData(DBaseCtrl::Mat_WholePrice, Qt::Horizontal, "Buy price");

    ui->materialsTable->setColumnWidth(DBaseCtrl::Mat_Name, NAME_COL_WIDTH);
    ui->materialsTable->setColumnWidth(DBaseCtrl::Mat_Brand, BRAND_COL_WIDTH);
    ui->materialsTable->setColumnWidth(DBaseCtrl::Mat_Description, DESC_COL_WIDTH);
    ui->materialsTable->setColumnWidth(DBaseCtrl::Mat_Price, SELL_COL_WIDTH);
    ui->materialsTable->setColumnWidth(DBaseCtrl::Mat_Quantity, QTY_COL_WIDTH);
    ui->materialsTable->setColumnWidth(DBaseCtrl::Mat_PartNum, PARTNUM_COL_WIDTH);
    ui->materialsTable->setColumnWidth(DBaseCtrl::Mat_WholePrice, BUY_COL_WIDTH);


    materialModel->select();

    ui->materialsTable->selectRow(0);

    connect(ui->materialsTable->horizontalHeader(),
            SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this,
            SLOT(scrollToSelection()));
}

MaterialPricesDialog::~MaterialPricesDialog()
{
    delete ui;
}

void MaterialPricesDialog::on_editButton_clicked()
{
    if(materialProxyModel->rowCount() > 0)
    {
        QModelIndex oldIndex = materialselectionModel->currentIndex();
        QModelIndex index = materialProxyModel->mapToSource(oldIndex);
        EditMaterialsChargeDialog editDialog(dbCtrl, index, this);
        editDialog.exec();

        QModelIndex last = materialProxyModel->mapFromSource(index);
        ui->materialsTable->selectRow(last.row());
    }
}

void MaterialPricesDialog::on_addButton_clicked()
{
    bool ok;
    QString newMaterialName = QInputDialog::getText(this, "New Material Price",
                                                    "Enter the Name of the new Material Type",
                                                    QLineEdit::Normal, "", &ok);

    if(ok && (newMaterialName.length() > 1))
    {
        int newRow = materialModel->rowCount();
        materialModel->insertRow(newRow);
        materialModel->setData(materialModel->index(newRow, 1),newMaterialName);
        materialModel->submit();

        ui->materialsTable->selectRow(materialProxyModel->mapFromSource(materialModel->index(newRow, 1)).row());

        on_editButton_clicked();
    }
}

void MaterialPricesDialog::on_searchEdit_textEdited(QString newText)
{
    QModelIndex firstIndex = ui->materialsTable->currentIndex();
    QModelIndex finalIndex = materialProxyModel->mapToSource(firstIndex);

    int col = ui->searchCombo->currentIndex() + 1;
    QString sortString = QString("^%1.*").arg(newText);

    materialProxyModel->setFilterRegExp(QRegExp(sortString, Qt::CaseInsensitive));
    materialProxyModel->setFilterKeyColumn(col);
    materialProxyModel->sort(1, Qt::AscendingOrder);

    int newIndex = materialProxyModel->mapFromSource(finalIndex).row();

    if(newIndex > 0)
        ui->materialsTable->selectRow(newIndex);
    else
        ui->materialsTable->selectRow(0);
}

void MaterialPricesDialog::on_searchCombo_currentIndexChanged(QString )
{
    ui->searchEdit->setFocus();
    on_searchEdit_textEdited(ui->searchEdit->text());
}

void MaterialPricesDialog::on_materialsTable_doubleClicked(QModelIndex )
{
    on_editButton_clicked();
}

/**
  * Make sure the selection stays in view
  */
void MaterialPricesDialog::scrollToSelection()
{
    ui->materialsTable->scrollTo(materialselectionModel->currentIndex());
}

void MaterialPricesDialog::on_deleteButton_clicked()
{
    QModelIndex currRow = materialProxyModel->mapToSource(materialselectionModel->currentIndex());

    materialModel->removeRow(currRow.row());
    materialModel->submitAll();
}
