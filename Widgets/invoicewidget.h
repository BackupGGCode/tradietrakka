#ifndef INVOICEWIDGET_H
#define INVOICEWIDGET_H

#define DATE_FORMAT "yyyyMMdd"

#include "dbasectrl.h"
#include "mainwindow.h"

#include <csqltablemodel.h>

#include <QDate>
#include <QWidget>
#include <QShowEvent>
#include <QSqlQueryModel>
#include <QTableWidgetItem>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>

#define SMALL_WIDTH 50
#define LARGE_WIDTH 100
#define TOTAL_WIDTH LARGE_WIDTH * 4 + SMALL_WIDTH

namespace Ui {
    class InvoiceWidget;
}

class InvoiceWidget : public QWidget
{
    Q_OBJECT

public:
    enum Status{ Awaiting = 1, Pending = 2, Paid = 3 };

private:
    Status status;
    bool locked;

    DBaseCtrl *dbCtrl;
    int invoiceID, addressID, entityID, entityType;
    bool entityChanged, descChanged, dateChanged;

    int lastRow;
    QImage invoiceStamp;
    bool readOnly;
    bool readWriteState;

    QString *contactName;
    QMenu *addButtonMenu;
    QSqlTableModel *contactModel;
    QSqlTableModel *addressModel;
    QSqlTableModel *invoiceModel;

    CSqlTableModel *materialsOnInvoiceModel;
    CSqlTableModel *labourOnInvoiceModel;


    QItemSelectionModel *materialsSelection, *labourSelection;

    QString tmpName, tmpBrand, tmpQuantity, tmpPrice;
    QModelIndex invoiceIndex;


//    QSqlQueryModel *companyAddressModel;

    enum ContactType
    {
        Company = 0,
        Person = 1
    };

public:

    InvoiceWidget(DBaseCtrl *dbCtrl, int invoiceID, QWidget *parent = 0);
    ~InvoiceWidget();

    QString getContactName(){ return *contactName; }

    InvoiceWidget::Status getStatus();
    void lockInvoice();
    void unlockInvoice();
    int getInvoiceID() { return invoiceID; }

private:
    Ui::InvoiceWidget *ui;

    void initButtonMenu();
    void parseContactInfo(ContactType contactType, int contactID, int addressID = -1);
    void loadInvoice();
    void loadMaterials();
    void loadLabour();
    void updateContact();
    void sumTotals();


signals:
    void updateOverDueNotice();

private slots:
    void on_dateEdit_2_dateChanged(QDate date);
    void on_dueDateEdit_dateChanged(QDate);
    void on_paymentCombo_currentIndexChanged(int index);
    void on_lockButton_clicked();
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

    void setReadOnly(bool);

};

#endif // INVOICEWIDGET_H
