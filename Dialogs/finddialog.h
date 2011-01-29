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
