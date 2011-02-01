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

#ifndef MATERIALPRICESDIALOG_H
#define MATERIALPRICESDIALOG_H

#include <QDialog>

#include "dbasectrl.h"

class QSqlTableModel;
class QItemSelectionModel;
class QSortFilterProxyModel;

namespace Ui {
    class MaterialPricesDialog;
}

class MaterialPricesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MaterialPricesDialog(DBaseCtrl* dbCtrl, QWidget *parent = 0);
    ~MaterialPricesDialog();

private:
    Ui::MaterialPricesDialog *ui;

    QSqlTableModel *materialModel;
    QSortFilterProxyModel *materialProxyModel;
    QItemSelectionModel *materialselectionModel;

    DBaseCtrl *dbCtrl;

private slots:
    void on_deleteButton_clicked();
    void on_materialsTable_doubleClicked(QModelIndex );
    void on_searchCombo_currentIndexChanged(QString );
    void on_searchEdit_textEdited(QString );
    void on_addButton_clicked();
    void on_editButton_clicked();
    void scrollToSelection();
};

#endif // MATERIALPRICESDIALOG_H
