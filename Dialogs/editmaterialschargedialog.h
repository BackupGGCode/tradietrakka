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

#ifndef EDITMATERIALSCHARGEDIALOG_H
#define EDITMATERIALSCHARGEDIALOG_H

#include <QDialog>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlField>

#include "addressdialog.h"
#include "dbasectrl.h"

namespace Ui {
    class EditMaterialsChargeDialog;
}

class EditMaterialsChargeDialog : public QDialog
{
    Q_OBJECT

    QSqlTableModel *model;
    QModelIndex index;
    QSqlRecord record;

public:
    explicit EditMaterialsChargeDialog(DBaseCtrl *dbCtrl, QModelIndex index, QWidget *parent = 0);
    ~EditMaterialsChargeDialog();

private:
    Ui::EditMaterialsChargeDialog *ui;

private slots:
    void on_buttonBox_accepted();
};

#endif // EDITMATERIALSCHARGEDIALOG_H
