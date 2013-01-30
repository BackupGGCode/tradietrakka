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

    //QPalette palette;
    //pixmap = QPixmap(":/BackGround");

    //palette.setBrush(this->backgroundRole(), QBrush(pixmap));
    //this->setPalette(palette);
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

