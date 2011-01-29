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
