#ifndef VIEWCONTACTDIALOG_H
#define VIEWCONTACTDIALOG_H

#include <QDialog>
#include <QSqlRecord>
#include "dbasectrl.h"


namespace Ui {
    class ViewContactDialog;
}

class ViewContactDialog : public QDialog
{
    Q_OBJECT

    QSqlRecord record;

    DBaseCtrl *dbCtrl;
    int entityID;

    QString html;


public:
    enum ViewType
    {
        Company = 0,
        Personal,
    };

    explicit ViewContactDialog(ViewType viewType, DBaseCtrl *dbCtrl,
                               int entityID, QWidget *parent = 0);



    ~ViewContactDialog();

private:
    Ui::ViewContactDialog *ui;
    void viewCompany();
    void viewPerson();
//    QString parseAddresses(QSqlQueryModel *entityAddressModel);
//    QString parseContacts(QSqlQueryModel *entityContactsModel);
//    QString parsePerson(QSqlRecord personRecord);




    ViewType viewType;

private slots:
    void on_pdfButton_clicked();
    void on_printButton_clicked();
    void on_closeButton_clicked();
};

#endif // VIEWCONTACTDIALOG_H
