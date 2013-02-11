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

#ifndef DBASECTRL_H
#define DBASECTRL_H

#include <QSqlDatabase>

#include "appsettings.h"

#include "csqltablemodel.h"

#include <QSqlTableModel>
#include <QStringList>
#include <QSqlRelationalTableModel>
#include <QObject>
#include <QFont>


class DBaseCtrl : public QObject
{
    Q_OBJECT

    QString dbaseName;
    QSqlDatabase dbase;
    AppSettings *appSettings;
    QSqlTableModel *personModel;
    QSqlTableModel *companyModel;
    QSqlTableModel *materialsModel;
    QSqlTableModel *labourModel;
    QSqlTableModel *quoteModel;
    QSqlTableModel *invoiceModel;
    QSqlTableModel *receiptModel;
    QSqlTableModel *expenseModel;

    QSqlRelationalTableModel *addressModel;
    QSqlTableModel *suburbModel;
    QSqlTableModel *stateModel;

    QSqlRelationalTableModel *companyContactsModel;

    QSqlRelationalTableModel *ownerModel;


public:
    explicit DBaseCtrl(AppSettings *appSettings, QObject *parent = 0);
    ~DBaseCtrl();

    enum
    {
        Company_Id = 0,
        Company_Name = 1,
        Company_Phone = 2,
        Company_Fax = 3,
        Company_Email = 4,
        Company_WWW = 5,
        Company_ABN = 6
    };

    enum
    {
        Address_ID = 0,
        Address_UnitNum = 1,
        Address_HouseNum = 2,
        Address_POBox = 3,
        Address_Street = 4,
        Address_Suburb = 5,
        Address_Postcode = 6,
        Address_City = 7,
        Address_State = 8
    };

    enum
    {
        Person_ID = 0,
        Person_FName = 1,
        Person_LName = 2,
        Person_HomePH = 3,
        Person_OfficePH = 4,
        Person_MobilePH = 5,
        Person_FAX = 6,
        Person_Email = 7,
        Person_ABN = 8
    };

    enum
    {
        Quote_ID = 0,
        Quote_QNum = 1,
        Quote_QDate = 2,
        Quote_ExpiryDate = 3,
        Quote_Details = 4,
        Quote_EntityType = 5,
        Quote_EntityID = 6,
        Quote_Address = 7,
    };

    enum
    {
        Invoice_ID = 0,
        Invoice_INum = 1,
        Invoice_IDate = 2,
        Invoice_Details = 3,
        Invoice_DueDate = 4,
        Invoice_PaidDate = 5,
        Invoice_EntityType = 6,
        Invoice_EntityId = 7,
        Invoice_Address = 8,
        Invoice_Status = 9
    };

    enum
    {
        Receipt_ID = 0,
        Receipt_RDate = 1,
        Receipt_Details = 2,
        Receipt_InvoiceID = 3
    };

    enum
    {
        Expense_ID = 0,
        Expense_EDate = 1,
        Expense_Qty = 2,
        Expense_Amount = 3,
        Expense_GST = 4,
        Expense_Name = 5,
        Expense_Desc = 6,
        Expense_GST_CHCK = 7
    };

    enum
    {
        Charge_ID = 0,
        Charge_Date = 1,
        Charge_Desc = 2,
        Charge_Expense = 3,
        Charge_GST = 4,
        Charge_ChargeType = 5,
        Charge_ChargeTypeID = 6
    };

    enum
    {
        MoQ_ID = 0,
        MoQ_QuoteID = 1,
        MoQ_Name = 2,
        MoQ_Brand = 3,
        MoQ_QTY = 4,
        MoQ_Price = 5,
        MoQ_VTotal = 6
    };

    enum
    {
        LoQ_ID = 0,
        LoQ_QuoteID = 1,
        LoQ_Name = 2,
        LoQ_Description = 3,
        LoQ_Hours = 4,
        LoQ_Rate = 5,
        LoQ_VTotal = 6
    };

    enum
    {
        MoI_ID = 0,
        MoI_InvoiceID = 1,
        MoI_Name = 2,
        MoI_Brand = 3,
        MoI_QTY = 4,
        MoI_Price = 5,
        MoI_VTotal = 6
    };

    enum
    {
        LoI_ID = 0,
        LoI_InvoiceID = 1,
        LoI_Name = 2,
        LoI_Description = 3,
        LoI_Hours = 4,
        LoI_Rate = 5,
        LoI_VTotal = 6
    };

    enum
    {
        Mat_ID = 0,
        Mat_Name = 1,
        Mat_Brand = 2,
        Mat_Description = 3,
        Mat_Price = 4,
        Mat_Quantity = 5,
        Mat_PartNum = 6,
        Mat_WholePrice = 7
    };

    enum
    {
        Labour_ID = 0,
        Labour_Name = 1,
        Labour_Desc = 2,
        Labour_Rate = 3
    };

    enum
    {
        State_ID = 0,
        State_Name = 1,
        State_ShortName = 2
    };

    enum
    {
        Owner_ID = 0,
        Owner_FName = 1,
        Owner_LName = 2,
        Owner_BusinessName = 3,
        Owner_ABN = 4,
        Owner_MobilePH = 5,
        Owner_OfficePH = 6,
        Owner_FAX = 7,
        Owner_Email = 8,
        Owner_WWW = 9,
        Owner_UnitNO = 10,
        Owner_StreetNO = 11,
        Owner_Street = 12,
        Owner_Suburb = 13,
        Owner_City = 14,
        Owner_State = 15,
        Owner_Postcode = 16,
        Owner_DBVer = 17,
        Owner_QuoteHeader = 18,
        Owner_QuoteFooter = 19,
        Owner_InvoiceHeader = 20,
        Owner_InvoiceFooter = 21,
        Owner_LogoImage = 22
    };


    QSqlDatabase getDbase();
    bool connect(const bool &createDb = false);
    void removeDatabase();
    bool isOpen();
    bool hasTables();

    bool createTables();

    QSqlTableModel* getPersonModel();
    QSqlTableModel* getCompanyModel();
    QSqlTableModel* getMaterialsModel();
    QSqlTableModel* getLabourModel();
    QSqlTableModel* getReceiptModel();
    QSqlTableModel* getExpenseModel();

    CSqlTableModel* getMaterialsOnQuoteModel(int quoteID);
    CSqlTableModel* getLabourOnQuoteModel(int quoteID);

    CSqlTableModel* getMaterialsOnInvoiceModel(int quoteID);
    CSqlTableModel* getLabourOnInvoiceModel(int quoteID);

    QSqlTableModel* getQuoteModel();
    QSqlTableModel* getInvoiceModel();

    QSqlRelationalTableModel* getAddressModel();
    QSqlTableModel* getSuburbModel();
    QSqlTableModel* getStateModel();

    QSqlQueryModel* getCompanyContactsModel(int companyId);
    QSqlQueryModel* getCompanyAddressesModel(int companyId);
    QSqlQueryModel *getPersonAddressModel(int personID);

    QSqlQueryModel *getSingleCompanyModel(int companyID);
    QSqlQueryModel *getSinglePersonModel(int personID);
    QSqlQueryModel *getSingleAddressModel(int addressID);

    QSqlQueryModel *getSingleQuoteModel(int quoteID);
    QSqlQueryModel *getSingleInvoiceModel(int invoiceID);
    QSqlQueryModel *getSingleReceiptModel(int receiptID);
    QSqlQueryModel *getSingleExpenseModel(int expenseID);

    QStringList getQuoteEmailAndNum(int quoteID);

    int getNumberOfOverDue();

    void initDBModels();

    void insertPersonAtCompany(int, int);
    void removePersonAtCompany(int);
    void insertAddressOfCompany(int, int);
    void removeAddressOfCompany(int);
    void insertAddressOfPerson(int, int);
    void removeAddressOfPerson(int, int);
    void removeAllAddressesOfPerson(int);

    bool removeQuote(int quoteID);

    QImage getLogo();
    void setLogo(QImage *logo);
    QSqlRelationalTableModel *getOwnerModel();

private:
    void initPersonModel();
    void initCompanyModel();
    void initAddressModel();
    void initStateModel();
    void initMaterialsModel();
    void initLabourModel();
    void initQuoteModel();
    void initInvoiceModel();
    void initReceiptModel();
    void initExpenseModel();
    void initQuoteBridgeModels();
    void initOwnerModel();

    void initCompanyContactsModel();
};

#endif // DBASECTRL_H
