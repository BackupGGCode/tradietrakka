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

#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#define DATE_FORMAT "yyyyMMdd"

#include <QDialog>
#include "dbasectrl.h"
#include <QSortFilterProxyModel>

namespace Ui {
    class FindDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT

    DBaseCtrl *dbCtrl;
    QSortFilterProxyModel *proxyModel;
    QSqlTableModel *tableModel;
    QSqlRelationalTableModel *companyRelationModel, *personRelationModel;

public:
    enum findType
    {
        Quote,
        Invoice,
        Expense,
        Receipt,
        Materials,
        Labour,
        OverDue
    };

    explicit FindDialog(findType dialogType, DBaseCtrl *dbCtrl, QWidget *parent = 0);
    ~FindDialog();


private:
    Ui::FindDialog *ui;

    findType currType;

    void initQuoteDialog();
    void initInvoiceDialog();
    void initExpenseDialog();
    void initReceiptDialog();
    void initMaterialsDialog();
    void initLabourDialog();
    void initOverDueDialog();
    void closeEvent(QCloseEvent *);

public slots:
    void on_searchCombo_currentIndexChanged(QString newText);
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void changeQuoteEntityType(int);
    void changeInvoiceEntityType(int);

private slots:
    void on_searchEdit_textEdited(QString );
};

#endif // FINDDIALOG_H
