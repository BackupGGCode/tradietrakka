#include "homewidget.h"
#include "ui_homewidget.h"

/**
  *
  */
HomeWidget::HomeWidget(QSqlDatabase dbase, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomeWidget)
{
    this->dbase = dbase;
    ui->setupUi(this);

    QPalette palette;
    pixmap = QPixmap(":/BackGround");

    palette.setBrush(this->backgroundRole(), QBrush(pixmap));
    this->setPalette(palette);
    setAutoFillBackground(true);
}

/**
  *
  */
HomeWidget::~HomeWidget()
{
    delete ui;
}

void HomeWidget::resizeEvent(QResizeEvent *)
{
    QPalette palette;
    if(this->width() > this->height())
    {
        pixmap = pixmap.scaledToWidth(this->width());
    } else {
        pixmap = pixmap.scaledToHeight(this->height());
    }

    palette.setBrush(this->backgroundRole(), QBrush(pixmap));
    this->setPalette(palette);
    setAutoFillBackground(true);
}

/**
  *
  */
void HomeWidget::on_businessButton_clicked()
{
    emit businessButtonClicked(ContactDialog::Business);
//    ContactDialog cd(ContactDialog::Business, dbase);
//    cd.exec();
}


/**
  *
  */
void HomeWidget::on_materialButton_clicked()
{
    emit materialsButtonClicked();
//    MaterialPricesDialog mp;
//    mp.exec();
}

/**
  *
  */
void HomeWidget::on_labourButton_clicked()
{
    emit labourButtonClicked();
//    LabourChargeDialog lc;
//    lc.exec();
}

/**
  *
  */
void HomeWidget::on_contactsButton_clicked()
{
    emit personalButtonClicked(ContactDialog::Personal);
//    ContactDialog cd(ContactDialog::Personal, dbase);
//    cd.exec();
}

