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
