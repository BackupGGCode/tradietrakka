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
