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

#ifndef LABOURCHARGEDIALOG_H
#define LABOURCHARGEDIALOG_H

#include <QDialog>

#include "dbasectrl.h"

class QSqlTableModel;
class QItemSelectionModel;
class QSortFilterProxyModel;
namespace Ui {
    class LabourChargeDialog;
}

class LabourChargeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LabourChargeDialog(DBaseCtrl* dbCtrl, QWidget *parent = 0);
    ~LabourChargeDialog();

private:
    Ui::LabourChargeDialog *ui;

    QSqlTableModel *labourModel;
    QSortFilterProxyModel *labourProxyModel;
    QItemSelectionModel *labourSelectionModel;

    DBaseCtrl *dbCtrl;

    int currColumn;

private slots:

    void on_deleteButton_clicked();
    void on_labourTable_doubleClicked(QModelIndex index);
    void on_comboBox_currentIndexChanged(QString );
    void on_lineEdit_textEdited(QString );
    void on_editButton_clicked();
    void on_addButton_clicked();
    void sortByColumn(int );
};

#endif // LABOURCHARGEDIALOG_H
