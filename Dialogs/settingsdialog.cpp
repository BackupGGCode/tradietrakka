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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QFontDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlRelationalDelegate>
#include <QSqlError>
#include <QTableView>

SettingsDialog::SettingsDialog(DBaseCtrl *dbCtrl, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->dbCtrl = dbCtrl;

    ownerModel = dbCtrl->getOwnerModel();

    QSqlTableModel *stateRelation = ownerModel->relationModel(DBaseCtrl::Owner_State);
    ui->stateCombo->setModel(stateRelation);
    ui->stateCombo->setModelColumn(DBaseCtrl::State_Name);
    ui->stateCombo->setCurrentIndex(0);

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setModel(ownerModel);
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));

    mapper->addMapping(ui->businessNameEdit, DBaseCtrl::Owner_BusinessName);
    mapper->addMapping(ui->fNameEdit, DBaseCtrl::Owner_FName);
    mapper->addMapping(ui->lNameEdit, DBaseCtrl::Owner_LName);
    mapper->addMapping(ui->abnEdit, DBaseCtrl::Owner_ABN);
    mapper->addMapping(ui->mobileEdit, DBaseCtrl::Owner_MobilePH);
    mapper->addMapping(ui->phoneEdit, DBaseCtrl::Owner_OfficePH);
    mapper->addMapping(ui->faxEdit, DBaseCtrl::Owner_FAX);
    mapper->addMapping(ui->emailEdit, DBaseCtrl::Owner_Email);
    mapper->addMapping(ui->wwwEdit, DBaseCtrl::Owner_WWW);

    mapper->addMapping(ui->unitEdit, DBaseCtrl::Owner_UnitNO);
    mapper->addMapping(ui->houseNumberEdit, DBaseCtrl::Owner_StreetNO);
    mapper->addMapping(ui->streetEdit, DBaseCtrl::Owner_Street);
    mapper->addMapping(ui->suburbEdit, DBaseCtrl::Owner_Suburb);
    mapper->addMapping(ui->postcodeEdit, DBaseCtrl::Owner_Postcode);
    mapper->addMapping(ui->cityEdit, DBaseCtrl::Owner_City);
    mapper->addMapping(ui->stateCombo, DBaseCtrl::Owner_State);

    mapper->addMapping(ui->quoteHeaderEdit, DBaseCtrl::Owner_QuoteHeader);
    mapper->addMapping(ui->quoteFooterEdit, DBaseCtrl::Owner_QuoteFooter);
    mapper->addMapping(ui->invoiceHeaderEdit, DBaseCtrl::Owner_InvoiceHeader);
    mapper->addMapping(ui->invoiceFooterEdit, DBaseCtrl::Owner_InvoiceFooter);

    if(ownerModel->rowCount() < 1)
    {
        ownerModel->insertRow(ownerModel->rowCount());
        ownerModel->submitAll();
    }

    mapper->toFirst();

    if(ui->stateCombo->currentIndex() < 0)
        ui->stateCombo->setCurrentIndex(0);

    logoImage = dbCtrl->getLogo();
    updateLogo();

    QFileInfo dbFile(appSettings.getDbPath() + QDir::separator() + appSettings.getDbName());
    ui->dbNameEdit->setText(dbFile.absoluteFilePath());

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_browseButton_clicked()
{
    QString filter = "Images (*.jpg *.png *.bmp)";
    QString logoFileName = QFileDialog::getOpenFileName(this, "Select logo file", QDir::homePath(), filter);

    if(logoFileName.isEmpty())
       return;

    QImage tempImage;
    if(!tempImage.load(logoFileName))
    {
        QMessageBox::warning(this, "Cannot load image", "The selected image cannot be loaded!");
        return;
    }

    QSize tmpSize = tempImage.size();
    if(tmpSize.height() > 200)
        logoImage = tempImage.scaled(tmpSize.width(), 200, Qt::KeepAspectRatio);
    else
        logoImage = tempImage;

    updateLogo();
}

void SettingsDialog::updateLogo()
{
    if(!logoImage.isNull())
    {
        ui->logoLabel->setPixmap(QPixmap::fromImage(logoImage));
        ui->logoLabel->setMinimumSize(logoImage.size());
        ui->logoLabel->setMaximumSize(logoImage.size());
    }
}

void SettingsDialog::on_clearButton_clicked()
{
    ui->logoLabel->clear();
    logoImage = QImage();
}

void SettingsDialog::commit()
{
    if(!logoImage.isNull())
        dbCtrl->setLogo(&logoImage);

    mapper->submit();
    ownerModel->submitAll();
}

void SettingsDialog::on_buttonBox_accepted()
{
    commit();
}

void SettingsDialog::on_buttonBox_rejected()
{
    mapper->revert();
}

void SettingsDialog::on_buttonBox_clicked(QAbstractButton* button)
{
    if(button->text() == "Apply")
        commit();
}

void SettingsDialog::on_changeDBButton_clicked()
{
    QMessageBox::warning(this, "Restart Will Be Required",
                         "TradieTrakka must be restarted after choosing a new Database file\n"
                         "IF you Click OK TradieTrakka will close after the new database is selected");

    QString dbFile = QFileDialog::getOpenFileName(this, "Choose TradieTrakka DB File",
                                                  QDir::homePath(), "TraddieTrakka DB (*.tt)");

    if(dbFile.isEmpty())
       return;

    if(!dbFile.endsWith(".tt"))
        dbFile.append(".tt");

    QFileInfo fileInfo(dbFile);
    appSettings.setDbPath(fileInfo.path());
    appSettings.setDbName(fileInfo.fileName());

    QApplication::exit();
}
