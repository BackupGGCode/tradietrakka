/*******************************************************************************
  Copyright (C) 2010  Craig Hume

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

#ifndef PRINTCOMPOSER_H
#define PRINTCOMPOSER_H

class QDate;
class QImage;
class DBaseCtrl;
class QSqlRecord;
class QSqlQueryModel;
#include <QString>


class PrintComposer
{

public:
    QString header, logo, footer, materialsTableHtml,
            labourTableHtml, endTableHtml, addressHeader, quoteHeader,
            quoteFooter, invoiceHeader, invoiceFooter;

    PrintComposer();
    enum OutputType {PDF, Print, Email};
    enum PrintType {Quote, Invoice, Receipt, Company, Person, BAS, PandL, Breakdown};
    enum TableType {Materials, Labour};

    static QStringList getEmailAndNum(int quoteID, DBaseCtrl *baseCtrl);
    static void printString(QString html, PrintComposer::OutputType outputType, QImage logo, QString mailAddr = "", QString invoiceNum = "");

    static QString quoteString(int quoteID, DBaseCtrl *dbCtrl, bool detail = true);
    static QString invoiceString(int invoiceID, DBaseCtrl *dbCtrl, bool detail = true);
//    static QString receiptString(int invoiceID, int receiptID, DBaseCtrl *dbCtrl);
    static QString receiptString(int receiptID, DBaseCtrl *dbCtrl);
    static QString expenseString(int expenseID, DBaseCtrl *dbCtrl);

    static QString basString(QDate startDate, QDate endDate, DBaseCtrl *dbCtrl);
    static QString pandlString(QDate startDate, QDate endDate, DBaseCtrl *dbCtrl);
    static QString breakdownString(QDate startDate, QDate endDate, DBaseCtrl *dbCtrl);
    static QString personalContact(int personalID, DBaseCtrl *dbCtrl);
    static QString companyContact(int companyID, DBaseCtrl *dbCtrl);

    //*****//
    QString parseAddresses(QSqlQueryModel *entityAddressModel, DBaseCtrl *dbCtrl);
    QString parseContacts(QSqlQueryModel *entityContactsModel, DBaseCtrl *dbCtrl);
    QString parsePerson(QSqlRecord personRecord);

    QString tableRow(QSqlRecord record, double *subTotal);
    QString toAddress(int addressID, int entityID, int entityType, DBaseCtrl *dbCtrl);
    QString totalToHtml(double total);
    QString dateHeader(QString entityName, QString entityNumber, QString entityDate, QString entityDueDate = "");
    QString descriptionHtml(QString description);
    QString titleHtml(QString name);
    QString footerHtml(QString footerData);
    QString basTable(double, double, double, double, double, QString, QString);
    QString basBreakdownTable(QString, QString, QString, double);
    QString pandlTotals(double total, double gstTotal);
    QString abnHtml(QString abn);
};

#endif // PRINTCOMPOSER_H


