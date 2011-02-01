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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QDataWidgetMapper>

#include "dbasectrl.h"
#include "appsettings.h"

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

    QImage logoImage;
    DBaseCtrl *dbCtrl;
    QSqlRelationalTableModel *ownerModel;
//    QSqlTableModel *stateRelation;

    QDataWidgetMapper *mapper;
    AppSettings appSettings;

public:
    explicit SettingsDialog(DBaseCtrl *dbCtrl, QWidget *parent = 0);
    ~SettingsDialog();

private:
    Ui::SettingsDialog *ui;
    void loadSettings();
    void updateLogo();
    void commit();

private slots:
    void on_changeDBButton_clicked();
    void on_buttonBox_clicked(QAbstractButton* button);
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_clearButton_clicked();
    void on_browseButton_clicked();
};

#endif // SETTINGSDIALOG_H
