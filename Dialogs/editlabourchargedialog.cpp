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
