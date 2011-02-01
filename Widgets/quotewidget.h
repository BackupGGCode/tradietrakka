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

#ifndef QUOTEWIDGET_H
#define QUOTEWIDGET_H

#include "mainwindow.h"
#include "dbasectrl.h"

#include <QWidget>
#include <QTableWidgetItem>
#include <QShowEvent>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QDate>

#include <QItemSelectionModel>

#include <csqltablemodel.h>

#define SMALL_WIDTH 50
#define LARGE_WIDTH 100
#define TOTAL_WIDTH LARGE_WIDTH * 4 + SMALL_WIDTH

namespace Ui {
    class QuoteWidget;
}

class QuoteWidget : public QWidget
{
    Q_OBJECT

    DBaseCtrl *dbCtrl;
    int quoteID, addressID, entityID, entityType;
    bool entityChanged, descChanged, dateChanged;

    QString *contactName;
    QMenu *addButtonMenu;
    QSqlTableModel *contactModel;
    QSqlTableModel *addressModel;
    QSqlTableModel *quoteModel;

    CSqlTableModel *materialsOnQuoteModel;
    CSqlTableModel *labourOnQuoteModel;


    QItemSelectionModel *materialsSelection, *labourSelection;

    QString tmpName, tmpBrand, tmpQuantity, tmpPrice;
    QModelIndex quoteIndex;

    enum ContactType
    {
        Company = 0,
        Person
    };

public:
    explicit QuoteWidget(DBaseCtrl *dbCtrl, int quoteID, QWidget *parent = 0);
    ~QuoteWidget();

    QModelIndex getQuoteIndex() { return quoteIndex; }

    int getQuoteId(){ return quoteID;}
    QString getContactName(){ return *contactName; }

private:
    Ui::QuoteWidget *ui;

    void initButtonMenu();
    void parseContactInfo(ContactType contactType, int contactID, int addressID = -1);
    void loadQuote();
    void loadMaterials();
    void loadLabour();
    void updateContact();
    void sumTotals();

private slots:
    void on_dateEdit_dateChanged(QDate );
    void contactsCompany();
    void contactsPersonal();
    void viewKeyPressed(QKeyEvent *event);
    void viewKeyPressedLabour(QKeyEvent *event);

    void cellChanged(QModelIndex, QModelIndex);
    void cellChangedLabour(QModelIndex, QModelIndex);

    void getLastRowData();
    void resizeDesc();
    void resizeMaterials();
    void resizeLabour();
    void materialsRightClick();
    void labourRightClick();

    void findMaterial();
    void deleteMaterial();
    void findLabour();
    void deleteLabour();

};

#endif // QUOTEWIDGET_H
