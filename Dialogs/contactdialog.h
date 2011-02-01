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

#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include "dbasectrl.h"

#include <QDialog>

class QAction;
class QSqlTableModel;
class QItemSelectionModel;
class QSortFilterProxyModel;

namespace Ui {
    class ContactDialog;
}

class ContactDialog : public QDialog
{
    Q_OBJECT

    DBaseCtrl *dbCtrl;
    QSortFilterProxyModel *proxyModel;
    QItemSelectionModel *selectionModel;
    QAction *companyAction, *personalAction;
    QSqlTableModel *companyModel, *personModel;

public:
    enum contactType
    {
        Business,
        Personal
    };

     ContactDialog(contactType, DBaseCtrl*, QWidget *parent = 0);
    ~ContactDialog();

private:
    Ui::ContactDialog *ui;

    void initBusiness();
    void initPersonal();
    contactType thisContactType;

private slots:
    void addCompanyContact();
    void scrollToSelection();
    void addPersonalContact();
    void on_editButton_clicked();
    void on_viewButton_clicked();
    void on_buttonBox_2_accepted();
    void on_searchEdit_textEdited(QString );
    void on_searchCombo_currentIndexChanged(int index);
};

#endif // CONTACTDIALOG_H
