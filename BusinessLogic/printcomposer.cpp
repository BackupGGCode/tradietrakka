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

#include "printcomposer.h"
#include "dbasectrl.h"

#include <QSqlError>
#include <QDate>
#include <QUrl>
//#include <QDebug>
#include <QPrinter>
#include <QSqlQuery>
#include <QFileDialog>
#include <QPrintDialog>
#include <QTextDocument>

/**
  *
  */
void PrintComposer::printString(QString html, PrintComposer::OutputType outputType, QImage logo)
{
    QPrinter printer;
    QTextDocument output;

    output.addResource(QTextDocument::ImageResource, QUrl("logo://logo"), QVariant(logo));

    //qDebug() << html;

    if(outputType == PrintComposer::Print)
    {
        QPrintDialog printDialog(&printer);
        if(printDialog.exec())
        {

            output.setHtml(html);
            output.print(&printer);
        }
    }
    else
    {
        printer.setOutputFormat(QPrinter::PdfFormat);
        QString fileName = QFileDialog::getSaveFileName(0, "Save PDF", QDir::homePath(), "*.pdf");
        if(!fileName.isEmpty())
        {
            if(!fileName.endsWith(".pdf"))
                printer.setOutputFileName(fileName + ".pdf");
            else
                printer.setOutputFileName(fileName);

            output.setHtml(html);
            output.print(&printer);
        }
    }
}

/**
  *
  */
QString PrintComposer::quoteString(int quoteID, DBaseCtrl *dbCtrl, bool detail)
{
    PrintComposer pc;

    QString html = pc.header + pc.logo;

    html += pc.abnHtml(dbCtrl->getOwnerModel()->record(0).value(dbCtrl->Owner_ABN).toString());

    html += pc.titleHtml("Quote");

    QString quoteNumber = QString::number(quoteID);


    QSqlQueryModel *quoteModel = dbCtrl->getSingleQuoteModel(quoteID);
    const QSqlRecord quoteRecord = quoteModel->record(quoteModel->rowCount() - 1);

    QString quoteDate = quoteRecord.value(DBaseCtrl::Quote_QDate).toString();

    html += pc.dateHeader("Quote", quoteNumber, quoteDate);

    html += pc.addressHeader;

    //Addresses
    html += pc.toAddress(quoteRecord.value(DBaseCtrl::Quote_Address).toInt(),
                             quoteRecord.value(DBaseCtrl::Quote_EntityID).toInt(),
                             quoteRecord.value(DBaseCtrl::Quote_EntityType).toInt(),
                             dbCtrl);
    html += pc.endTableHtml;

    //Description
//    html +=  pc.descriptionHtml(quoteRecord.value(DBaseCtrl::Quote_Details).toString());

    QString description = pc.quoteHeader + "\n" +
                          quoteRecord.value(DBaseCtrl::Quote_Details).toString();

    html +=  pc.descriptionHtml(description);


    //materials tables
    if(detail)
        html += pc.materialsTableHtml;

    QSqlTableModel *materialOnQuoteModel= dbCtrl->getMaterialsOnQuoteModel(quoteID);
    double tmpSubTotal, subTotal = 0.0;
    for(int row = 0; row < materialOnQuoteModel->rowCount(); row++)
    {
        if(detail)
            html += pc.tableRow(materialOnQuoteModel->record(row), &tmpSubTotal);
        else
            pc.tableRow(materialOnQuoteModel->record(row), &tmpSubTotal);

        subTotal += tmpSubTotal;
    }
    if(detail)
        html += pc.endTableHtml;

//    //labour table
    if(detail)
        html += pc.labourTableHtml;

    QSqlTableModel *labourOnQuoteModel = dbCtrl->getLabourOnQuoteModel(quoteID);
    for(int row = 0; row < labourOnQuoteModel->rowCount(); row++)
    {
        if(detail)
            html += pc.tableRow(labourOnQuoteModel->record(row), &tmpSubTotal);
        else
            pc.tableRow(labourOnQuoteModel->record(row), &tmpSubTotal);

        subTotal += tmpSubTotal;
    }

    if(detail)
        html += pc.endTableHtml;

    html += pc.totalToHtml(subTotal);

    html += pc.quoteFooter;

    html += pc.footer;

    return html;
}

QString PrintComposer::invoiceString(int invoiceID, DBaseCtrl *dbCtrl, bool detail)
{
    PrintComposer pc;

    QString html = pc.header + pc.logo;

    html += pc.abnHtml(dbCtrl->getOwnerModel()->record(0).value(dbCtrl->Owner_ABN).toString());

    html += pc.titleHtml("Tax Invoice");

    QString invoiceNumber = QString::number(invoiceID);


    QSqlQueryModel *invoiceModel = dbCtrl->getSingleInvoiceModel(invoiceID);
    const QSqlRecord invoiceRecord = invoiceModel->record(invoiceModel->rowCount() - 1);

    QString invoiceDate = invoiceRecord.value(DBaseCtrl::Invoice_IDate).toString();
    QString invoiceDueDate = invoiceRecord.value(DBaseCtrl::Invoice_DueDate).toString();

    html += pc.dateHeader("Invoice", invoiceNumber, invoiceDate, invoiceDueDate);

    html += pc.addressHeader;

    //Addresses
    html += pc.toAddress(invoiceRecord.value(DBaseCtrl::Invoice_Address).toInt(),
                             invoiceRecord.value(DBaseCtrl::Invoice_EntityId).toInt(),
                             invoiceRecord.value(DBaseCtrl::Invoice_EntityType).toInt(),
                             dbCtrl);
    html += pc.endTableHtml;

    //Description

    QString description = pc.invoiceHeader + "\n" +
                          invoiceRecord.value(DBaseCtrl::Invoice_Details).toString();
    html +=  pc.descriptionHtml(description);

    //materials tables
    if(detail)
        html += pc.materialsTableHtml;

    QSqlTableModel *materialOnInvoiceModel = dbCtrl->getMaterialsOnInvoiceModel(invoiceID);
    double tmpSubTotal, subTotal = 0.0;
    for(int row = 0; row < materialOnInvoiceModel->rowCount(); row++)
    {
        if(detail)
            html += pc.tableRow(materialOnInvoiceModel->record(row), &tmpSubTotal);
        else
            pc.tableRow(materialOnInvoiceModel->record(row), &tmpSubTotal);

        subTotal += tmpSubTotal;
    }

    if(detail)
        html += pc.endTableHtml;

//    //labour table
    if(detail)
        html += pc.labourTableHtml;

    QSqlTableModel *labourOnInvoiceModel = dbCtrl->getLabourOnInvoiceModel(invoiceID);
    for(int row = 0; row < labourOnInvoiceModel->rowCount(); row++)
    {
        if(detail)
            html += pc.tableRow(labourOnInvoiceModel->record(row), &tmpSubTotal);
        else
            pc.tableRow(labourOnInvoiceModel->record(row), &tmpSubTotal);

        subTotal += tmpSubTotal;
    }

    if(detail)
        html += pc.endTableHtml;

    html += pc.totalToHtml(subTotal);

    html += pc.invoiceFooter;

    html += pc.footer;

    return html;
}



/**
  *
  */
QString PrintComposer::receiptString(int receiptID, DBaseCtrl *dbCtrl)
{
    PrintComposer pc;

    QString html = pc.header + pc.logo;

    html += pc.abnHtml(dbCtrl->getOwnerModel()->record(0).value(dbCtrl->Owner_ABN).toString());

    html += pc.titleHtml("Receipt");

    QString receiptNumber = QString::number(receiptID);

    const QSqlRecord receiptRecord = dbCtrl->getSingleReceiptModel(receiptID)->record(0);

    int invoiceID = receiptRecord.value(DBaseCtrl::Receipt_InvoiceID).toInt();
    QString receiptDate = receiptRecord.value(DBaseCtrl::Receipt_RDate).toString();

    QSqlQueryModel *invoiceModel = dbCtrl->getSingleInvoiceModel(invoiceID);
    const QSqlRecord invoiceRecord = invoiceModel->record(invoiceModel->rowCount() - 1);

    html += pc.dateHeader("Receipt", receiptNumber, receiptDate);

    html += pc.addressHeader;

    //Addresses
    html += pc.toAddress(invoiceRecord.value(DBaseCtrl::Invoice_Address).toInt(),
                             invoiceRecord.value(DBaseCtrl::Invoice_EntityId).toInt(),
                             invoiceRecord.value(DBaseCtrl::Invoice_EntityType).toInt(),
                             dbCtrl);
    html += pc.endTableHtml;

    html += "<center><h1 style='color:red'>PAID IN FULL</h1></center>";


    //Description
    html +=  pc.descriptionHtml(invoiceRecord.value(DBaseCtrl::Invoice_Details).toString());

    //materials tables
    html += pc.materialsTableHtml;

    QSqlTableModel *materialOnInvoiceModel = dbCtrl->getMaterialsOnInvoiceModel(invoiceID);
    double tmpSubTotal, subTotal = 0.0;
    for(int row = 0; row < materialOnInvoiceModel->rowCount(); row++)
    {
        html += pc.tableRow(materialOnInvoiceModel->record(row), &tmpSubTotal);
        subTotal += tmpSubTotal;
    }
    html += pc.endTableHtml;

//    //labour table
    html += pc.labourTableHtml;

    QSqlTableModel *labourOnInvoiceModel = dbCtrl->getLabourOnInvoiceModel(invoiceID);
    for(int row = 0; row < labourOnInvoiceModel->rowCount(); row++)
    {
        html += pc.tableRow(labourOnInvoiceModel->record(row), &tmpSubTotal);
        subTotal += tmpSubTotal;
    }

    html += pc.endTableHtml;

    html += pc.totalToHtml(subTotal);

    html += pc.footer;

    return html;
}

QString PrintComposer::expenseString(int expenseID, DBaseCtrl *dbCtrl)
{
    PrintComposer pc;

    QString html = pc.header + pc.logo + pc.titleHtml("Expense");
    QSqlRecord expenseRecord = dbCtrl->getSingleExpenseModel(expenseID)->record(0);

    QString item = expenseRecord.value(DBaseCtrl::Expense_Name).toString();
    QString date = QDate::fromString(expenseRecord.value(DBaseCtrl::Expense_EDate).toString(), "yyyyMMdd").toString("dd-MMM-yyyy");
    int qty = expenseRecord.value(DBaseCtrl::Expense_Qty).toInt();
    double price = expenseRecord.value(DBaseCtrl::Expense_Amount).toDouble();
    double gst = expenseRecord.value(DBaseCtrl::Expense_GST).toDouble();
    double total = price * qty;
    QString description = expenseRecord.value(DBaseCtrl::Expense_Desc).toString();

    html += "<table width='100%'>";

    html += "<tr><td align='right' width='10%'>Item: </td>"
            "<td align='left'width='90%' colspan='4'>" +
             item + "</td></tr>";

    html += "<tr><td align='right' width='10%'>Date: </td>"
            "<td align='left' width='40%'>" + date + "</td>"
            "<td width='auto'></td>"
            "<td align='right' width='10%'>QTY: </td>"
            "<td align='left' width='40%'>" + QString::number(qty) + "</td></tr>";

    html += "<tr><td width='auto' colspan='3'></td>"
            "<td align='right' width='10%'>Price: </td>"
            "<td align='left' width='40%'>" + QString::number(price, 'f', 2) + "</td></tr>";

    html += "<tr><td width='auto' colspan='3'></td>"
            "<td align='right' width='10%'>GST: </td>"
            "<td align='left' width='40%'>" + QString::number(gst, 'f', 2) + "</td></tr>";

    html += "<tr><td width='auto' colspan='3'></td>"
            "<td align='right' width='10%'>Total: </td>"
            "<td align='left' width='40%'>" + QString::number(total, 'f', 2) + "</td></tr>";

    html += pc.endTableHtml;

    html += pc.descriptionHtml(description);

    html += pc.footer;


    return html;

}

/**
  *
  */
QString PrintComposer::basString(QDate startDate, QDate endDate, DBaseCtrl *dbCtrl)
{

    double totalSales = 0;
    double totalPurchases = 0;
    double gstOnSales = 0;
    double gstOnPurchases = 0;
    double paymentRefund = 0;

    // Get all Materials on Invoices for paid invoices in the specified period.
    QSqlQuery matQuery(dbCtrl->getDbase());
    QString prepMatQuery = "SELECT mat.quantity, mat.price FROM Invoice, MaterialsOnInvoice as mat "
                           "WHERE mat.invoiceID = Invoice.id AND "
                           "Invoice.paidDate >= " + startDate.toString("yyyyMMdd") + " AND "
                           "Invoice.paidDate <= " + endDate.toString("yyyyMMdd") + " AND "
                           "Invoice.status = 3";

    matQuery.exec(prepMatQuery);

    while(matQuery.next())
    {
        totalSales += matQuery.value(1).toDouble() * matQuery.value(0).toDouble();
        gstOnSales += matQuery.value(1).toDouble() * matQuery.value(0).toDouble() / 11;
    }

    // Get all Labour on Inoivces for paid invoices in the specified period.
    QSqlQuery labQuery(dbCtrl->getDbase());
    QString prepLabQuery = "SELECT lab.hours, lab.rate FROM Invoice, LabourOnInvoice as lab "
                           "WHERE lab.invoiceID = Invoice.id AND "
                           "Invoice.IDate >= " + startDate.toString("yyyyMMdd") + " AND "
                           "Invoice.IDate <= " + endDate.toString("yyyyMMdd") + " AND "
                           "Invoice.status = 3";

    labQuery.exec(prepLabQuery);

    while(labQuery.next())
    {
        totalSales += labQuery.value(1).toDouble() * labQuery.value(0).toDouble();
        gstOnSales += labQuery.value(1).toDouble() * labQuery.value(0).toDouble() / 11;
    }

    // Get all expenses in the specified period.
    QSqlQuery expenseQuery(dbCtrl->getDbase());
    QString prepareQuery = "SELECT quantity, amount, gst FROM Expense "
                           "WHERE eDate >= " + startDate.toString("yyyyMMdd") +
                           " AND eDate <= " + endDate.toString("yyyyMMdd");

    expenseQuery.exec(prepareQuery);

    while(expenseQuery.next())
    {
        totalPurchases += expenseQuery.value(1).toInt();
        gstOnPurchases += expenseQuery.value(2).toInt();
    }

    int tmpGstSales = gstOnSales * 100;
    gstOnSales = tmpGstSales / 100.00;

    paymentRefund = gstOnSales - gstOnPurchases;

    QString paymentOrRefund = "This amount is payable to the ATO (9): ";
    QString returnPaymentColour = "style='color:red'";

    if(paymentRefund < 0)
    {
        paymentOrRefund ="This amount is refundable to you (9): ";
        paymentRefund *= -1;
        returnPaymentColour = "style='color:green'";
    }

    PrintComposer pc;

    QString html = pc.header + pc.logo + pc.titleHtml("BAS Report");

    html += "<center><h2>For the periods of <br />" +
            startDate.toString("dd-MMM-yyyy") + " to " +
            endDate.toString("dd-MMM-yyyy") + "</h2></center><br />";

    html += pc.basTable(totalSales, totalPurchases,
                        gstOnSales, gstOnPurchases,
                        paymentRefund, returnPaymentColour,
                        paymentOrRefund);

    html += pc.footer;

    return html;
}

/**
  *
  */
QString PrintComposer::pandlString(QDate startDate, QDate endDate, DBaseCtrl *dbCtrl)
{
    double totalSales = 0;
    double totalPurchases = 0;
    double gstOnSales = 0;
    double gstOnPurchases = 0;

    double paymentRefund = 0;

    // Get all Materials on Invoices for paid invoices in the specified period.
    QSqlQuery matQuery(dbCtrl->getDbase());
    QString prepMatQuery = "SELECT mat.quantity, mat.price FROM Invoice, MaterialsOnInvoice as mat "
                           "WHERE mat.invoiceID = Invoice.id AND "
                           "Invoice.paidDate >= " + startDate.toString("yyyyMMdd") + " AND "
                           "Invoice.paidDate <= " + endDate.toString("yyyyMMdd") + " AND "
                           "Invoice.status = 3";

    matQuery.exec(prepMatQuery);

    while(matQuery.next())
    {
        totalSales += matQuery.value(1).toDouble() * matQuery.value(0).toDouble();
    }

    // Get all Labour on Inoivces for paid invoices in the specified period.
    QSqlQuery labQuery(dbCtrl->getDbase());
    QString prepLabQuery = "SELECT lab.hours, lab.rate FROM Invoice, LabourOnInvoice as lab "
                           "WHERE lab.invoiceID = Invoice.id AND "
                           "Invoice.IDate >= " + startDate.toString("yyyyMMdd") + " AND "
                           "Invoice.IDate <= " + endDate.toString("yyyyMMdd") + " AND "
                           "Invoice.status = 3";

    labQuery.exec(prepLabQuery);

    while(labQuery.next())
    {
        totalSales += labQuery.value(1).toDouble() * labQuery.value(0).toDouble();
    }

    // Get all expenses in the specified period.
    QSqlQuery expenseQuery(dbCtrl->getDbase());
    QString prepareQuery = "SELECT quantity, amount, gst FROM Expense "
                           "WHERE eDate >= " + startDate.toString("yyyyMMdd") +
                           " AND eDate <= " + endDate.toString("yyyyMMdd");

    expenseQuery.exec(prepareQuery);

    while(expenseQuery.next())
    {
        totalPurchases += expenseQuery.value(1).toInt();
        gstOnPurchases += expenseQuery.value(2).toInt();
    }

    int tmpGstSales = gstOnSales * 100;
    gstOnSales = tmpGstSales / 100.00;

    paymentRefund = totalSales - totalPurchases;

    PrintComposer pc;

    QString html = pc.header + pc.logo + pc.titleHtml("Profit And Loss Report");


    html += "<center><h2>For the periods of <br />" +
            startDate.toString("dd-MMM-yyyy") + " to " +
            endDate.toString("dd-MMM-yyyy") + "</h2></center><br />";

    html += "<table width='90%' align='center'>";

    html += "<tr><td colspan='2' width='100%'"
            "style='background-color:lightgrey'"
            "align='center'>Totals</td></tr>";

    html += "<tr><td align='right' width='50%'>Total Purchases: </td><td align='left'>$" +
            QString::number(totalPurchases, 'f', 2) + "</td></tr>";

    html += "<tr><td align='right' width='50%'>Total Sales: </td><td align='left'>$" +
            QString::number(totalSales, 'f', 2) + "</td></tr>";

    html += "<tr></tr><tr><td colspan='2' width='100%'"
            "style='background-color:lightgrey'"
            "align='center'>Profit or Loss</td></tr>";

    if(paymentRefund >= 0)
    {
        html += "<tr><td align='right' width='50%' style='color:green'>"
                "<h3>Profit for the period is: </h3>"
                "</td><td align='left' style='color:green'><h3>$" +
                QString::number(paymentRefund, 'f', 2) + "</h3></td></tr>";
    }
    else
    {
        html += "<tr><td align='right' width='50%' style='color:red'>"
                "<h3>Loss for the period is: </h3>"
                "</td><td align='left' style='color:red'><h3>$" +
                QString::number((paymentRefund * -1), 'f', 2) + "</td></tr>";
    }

    html += pc.endTableHtml;

    html += pc.footer;

    return html;
}

/**
  *
  */
QString PrintComposer::breakdownString(QDate startDate, QDate endDate, DBaseCtrl *dbCtrl)
{
    PrintComposer pc;

    QStringList materialsList, labourList, expenseList;

    double totalSales = 0;
    double totalPurchases = 0;
    double gstOnSales = 0;
    double gstOnPurchases = 0;

    double paymentRefund = 0;

// Get all Materials on Invoices for paid invoices in the specified period.
    QSqlQuery matQuery(dbCtrl->getDbase());
    QString prepMatQuery = "SELECT mat.name, mat.quantity, mat.price FROM Invoice, MaterialsOnInvoice as mat "
                           "WHERE mat.invoiceID = Invoice.id AND "
                           "Invoice.paidDate >= " + startDate.toString("yyyyMMdd") + " AND "
                           "Invoice.paidDate <= " + endDate.toString("yyyyMMdd") + " AND "
                           "Invoice.status = 3";

    matQuery.exec(prepMatQuery);

    double materialsTotal = 0.0, materialsGST = 0.0;

    while(matQuery.next())
    {
        materialsList << pc.basBreakdownTable(matQuery.value(0).toString(),
                                              matQuery.value(1).toString(),
                                              matQuery.value(2).toString(),
                                              matQuery.value(2).toDouble());

        materialsTotal += matQuery.value(2).toDouble() * matQuery.value(1).toDouble();
        materialsGST += matQuery.value(2).toDouble() * matQuery.value(1).toDouble() / 11;
    }
    totalSales += materialsTotal;
    gstOnSales += materialsGST;

// Get all Labour on Inoivces for paid invoices in the specified period.
    QSqlQuery labQuery(dbCtrl->getDbase());
    QString prepLabQuery = "SELECT lab.name, lab.hours, lab.rate FROM Invoice, LabourOnInvoice as lab "
                           "WHERE lab.invoiceID = Invoice.id AND "
                           "Invoice.IDate >= " + startDate.toString("yyyyMMdd") + " AND "
                           "Invoice.IDate <= " + endDate.toString("yyyyMMdd") + " AND "
                           "Invoice.status = 3";

    labQuery.exec(prepLabQuery);

    double labourTotal = 0.0, labourGST = 0.0;

    while(labQuery.next())
    {
        labourList << pc.basBreakdownTable(labQuery.value(0).toString(),
                                           labQuery.value(1).toString(),
                                           labQuery.value(2).toString(),
                                           labQuery.value(2).toDouble());

        labourTotal += labQuery.value(2).toDouble() * labQuery.value(1).toDouble();
        labourGST += labQuery.value(2).toDouble() * labQuery.value(1).toDouble() / 11;
    }
    totalSales += labourTotal;
    gstOnSales += labourGST;

// Get all expenses in the specified period.
    QSqlQuery expenseQuery(dbCtrl->getDbase());
    QString prepareQuery = "SELECT name, quantity, amount, gst FROM Expense "
                           "WHERE eDate >= " + startDate.toString("yyyyMMdd") +
                           " AND eDate <= " + endDate.toString("yyyyMMdd");

    expenseQuery.exec(prepareQuery);

    double expensesTotal = 0.0, expensesGST = 0.0;
    while(expenseQuery.next())
    {
        expenseList << pc.basBreakdownTable(  expenseQuery.value(0).toString(),
                                              expenseQuery.value(1).toString(),
                                              expenseQuery.value(2).toString(),
                                              expenseQuery.value(3).toDouble());

        expensesTotal += expenseQuery.value(2).toInt();
        expensesGST += expenseQuery.value(3).toInt();
    }
    totalPurchases += expensesTotal;
    gstOnPurchases += expensesGST;

    int tmpGstSales = gstOnSales * 100;
    gstOnSales = tmpGstSales / 100.00;

    paymentRefund = gstOnSales - gstOnPurchases;

    QString paymentOrRefund = "This amount is payable to the ATO (9): ";
    QString returnPaymentColour = "style='color:red'";

    if(paymentRefund < 0)
    {
        paymentOrRefund ="This amount is refundable to you (9): ";
        paymentRefund *= -1;
        returnPaymentColour = "style='color:green'";
    }


    //start of html
    QString html = pc.header + pc.logo + pc.titleHtml("BAS Breakdown Report");

    html += "<center><h2>For the periods of <br />" +
            startDate.toString("dd-MMM-yyyy") + " to " +
            endDate.toString("dd-MMM-yyyy") + "</h2></center><br />";

    html += "<table width='90%' align='center'>";

    html += "<tr style='background-color:lightgrey'>"
            "<td width='55%'>Materials</td>"
            "<td width='15%'>QTY</td>"
            "<td width='15%'>Amount</td>"
            "<td width='15%'>GST</td> </tr>";
    for(int i = 0; i < materialsList.count(); i++)
        html += materialsList[i];

    html += pc.pandlTotals(materialsTotal, materialsGST);

    html += "<tr></tr><tr style='background-color:lightgrey'>"
            "<td width='55%'>Labour</td>"
            "<td width='15%'>QTY</td>"
            "<td width='15%'>Amount</td>"
            "<td width='15%'>GST</td> </tr>";
    for(int i = 0; i < labourList.count(); i++)
        html += labourList[i];

    html += pc.pandlTotals(labourTotal, labourGST);

    html += "<tr></tr><tr style='background-color:lightgrey'>"
            "<td width='55%'>Expenses</td>"
            "<td width='15%'>QTY</td>"
            "<td width='15%'>Amount</td>"
            "<td width='15%'>GST</td> </tr>";
    for(int i = 0; i < expenseList.count(); i++)
        html += expenseList[i];

    html += pc.pandlTotals(expensesTotal, expensesGST);

    html += pc.endTableHtml;

    html += "<center><h2>BAS Report</h2></center>";

    html += pc.basTable(totalSales, totalPurchases,
                        gstOnSales, gstOnPurchases,
                        paymentRefund, returnPaymentColour,
                        paymentOrRefund);

    html += pc.footer;

    return html;
}

/**
  *
  */
QString PrintComposer::personalContact(int personalID, DBaseCtrl *dbCtrl)
{
    PrintComposer pc;
    QSqlQueryModel *person = dbCtrl->getSinglePersonModel(personalID);
    QSqlRecord personRecord = person->record(0);

    QString personalContactHtml = "<table width='400'>";

    personalContactHtml += "<tr><td colspan=2 align='center' style='background-color:lightgrey;color:black'>Person Details</td></tr>";
    personalContactHtml += pc.parsePerson(personRecord);
    personalContactHtml += "</table>";

    personalContactHtml += pc.parseAddresses(dbCtrl->getPersonAddressModel(personalID), dbCtrl);
    personalContactHtml += "</table>";

    return personalContactHtml;
}

/**
  *
  */
QString PrintComposer::companyContact(int companyID, DBaseCtrl *dbCtrl)
{
    PrintComposer pc;
    // get business table extract record
    QSqlQueryModel *company = dbCtrl->getSingleCompanyModel(companyID);
    //get basic company info
    QSqlRecord companyRecord = company->record(0);

    QString name, phone, fax, email, www, abn, companyContactHtml;

    name = companyRecord.value(DBaseCtrl::Company_Name).toString();
    phone = companyRecord.value(DBaseCtrl::Company_Phone).toString();
    fax = companyRecord.value(DBaseCtrl::Company_Fax).toString();
    email = companyRecord.value(DBaseCtrl::Company_Email).toString();
    www = companyRecord.value(DBaseCtrl::Company_WWW).toString();
    abn = companyRecord.value(DBaseCtrl::Company_ABN).toString();

    companyContactHtml = "<table width='400'>";

    companyContactHtml += "<tr><td colspan=2 align='center' style='background-color:lightgrey;color:black'>" +
            name + " Details</td></tr>";

    if(!phone.isEmpty())
    {
        companyContactHtml += "<tr><td align='right' width='100' style='color:blue'>Phone Number: </td><td>";
        companyContactHtml += phone + "</td></tr>";
    }

    if(!fax.isEmpty())
    {
        companyContactHtml += "<tr><td align='right' width='100' style='color:blue'>Fax Number: </td><td>";
        companyContactHtml += fax + "</td></tr>";
    }

    if(!email.isEmpty())
    {
        companyContactHtml += "<tr><td align='right' width='100' style='color:blue'>Email: </td><td>";
        companyContactHtml +=  email + "</td></tr>";
    }

    if(!www.isEmpty())
    {
        companyContactHtml += "<tr><td align='right' width='100' style='color:blue'>Website: </td><td>";
        companyContactHtml +=  www + "</td></tr>";
    }

    if(!abn.isEmpty())
    {
        companyContactHtml += "<tr><td align='right' width='100' style='color:blue'>ABN: </td><td>";
        companyContactHtml +=  abn + "</td></tr>";
    }
    companyContactHtml += "</table>";

    //get company address info
    companyContactHtml += pc.parseAddresses(dbCtrl->getCompanyAddressesModel(companyID), dbCtrl);

    //get company contacts info
    companyContactHtml += pc.parseContacts(dbCtrl->getCompanyContactsModel(companyID), dbCtrl);


    return companyContactHtml;
}

//*************
//** Object functions
/**
  *
  */
PrintComposer::PrintComposer()
{
    header = "<html><body>";
    logo = "<img src='logo://logo'></img>";

    footer = "</body></html>";

    materialsTableHtml = "<h4>Materials Charges</h4>";
    materialsTableHtml += "<table width='100%'>";
    materialsTableHtml += "<th><td width='30%' style='background-color:lightgrey;color:black'>Name</td>"
                          "<td width='30%' style='background-color:lightgrey;color:black'>Brand</td>"
                          "<td width='10%' style='background-color:lightgrey;color:black'>QTY</td>"
                          "<td width='15%' style='background-color:lightgrey;color:black'>Price</td>"
                          "<td width='15%' style='background-color:lightgrey;color:black'>Sub Total</td></th>";

    labourTableHtml = "<h4>Labour Charges</h4>";
    labourTableHtml += "<table width='100%'>";
    labourTableHtml += "<th><td width='30%' style='background-color:lightgrey;color:black'>Name</td>"
                       "<td width='30%' style='background-color:lightgrey;color:black'>Desc</td>"
                       "<td width='10%' style='background-color:lightgrey;color:black'>Hours</td>"
                       "<td width='15%' style='background-color:lightgrey;color:black'>Rate</td>"
                       "<td width='15%' style='background-color:lightgrey;color:black'>Sub Total</td></th>";

    endTableHtml = "</table><br />";

    addressHeader = "<table width='100%'>";

    addressHeader += "<tr><td align='left' style='background-color:lightgrey;color:black'>TO: </td>"
                     "<td></td>"
                     "<td align='left' style='background-color:lightgrey;color:black'>FROM: </td></tr>";
}

QString PrintComposer::titleHtml(QString name)
{
    return "<center><h1>" + name + "</h1></center><br />";
}

QString PrintComposer::footerHtml(QString footerData)
{
    return "<p>" + footerData.replace("\n", "<br />") + "</p>";
}

/**
  *
  */
QString PrintComposer::parseAddresses(QSqlQueryModel *entityAddressModel, DBaseCtrl *dbCtrl)
{
    QString addressHtml;

    QSqlTableModel *stateModel = dbCtrl->getStateModel();

    if(entityAddressModel->rowCount() > 0)
    {
        QString poBox, unitNum, streetNum, postCode, street, suburb, city, state;

        addressHtml = "<table width='400'>";
        addressHtml += "<tr><td colspan=2 align='center' "
                       "style='background-color:lightgrey;color:black'>Address Details</td></tr>";

        QSqlRecord addressRecord;
        int addressID, stateID;

        for(int row = 0; row < entityAddressModel->rowCount(); row++)
        {
            addressID = entityAddressModel->record(row).value(0).toInt();
            addressRecord = dbCtrl->getSingleAddressModel(addressID)->record(0);

            poBox = addressRecord.value(DBaseCtrl::Address_POBox).toString();
            unitNum = addressRecord.value(DBaseCtrl::Address_UnitNum).toString();
            streetNum = addressRecord.value(DBaseCtrl::Address_HouseNum).toString();
            postCode = addressRecord.value(DBaseCtrl::Address_Postcode).toString();
            street = addressRecord.value(DBaseCtrl::Address_Street).toString();
            suburb = addressRecord.value(DBaseCtrl::Address_Suburb).toString();
            city = addressRecord.value(DBaseCtrl::Address_City).toString();

            //state
            stateID = addressRecord.value(DBaseCtrl::Address_State).toInt();

            for(int i = 0; i < stateModel->rowCount(); i++)
                if(stateModel->record(i).value(DBaseCtrl::State_ID).toInt() == stateID)
                    state = stateModel->record(i).value(DBaseCtrl::State_ShortName).toString();

            if(row > 0)
                addressHtml += "<hr />";
            addressHtml += "<tr><td align='right' width='100' style='color:blue'>Address: </td><td>";

            if(!poBox.isEmpty())
            {
                addressHtml += "PO Box: " + poBox + "</td></tr>";
            }
            else
            {
                if(!unitNum.isEmpty())
                    addressHtml += unitNum + "/";

                addressHtml += streetNum + " ";
                addressHtml += street + "</td></tr>" ;
            }
            addressHtml += "<tr><td align='right' width='100' style='color:blue'></td><td>";
            addressHtml += suburb + ", " + postCode + "</td></tr>";

            addressHtml += "<tr><td align='right' width='100' style='color:blue'></td><td>";
            addressHtml += city + "</td></tr>";

            addressHtml += "<tr><td align='right' width='100' style='color:blue'></td><td>";
            addressHtml += state + "</td></tr>";

        }
        addressHtml += "</table>";
    }
    else
        return "";

    return addressHtml;
}

/**
  *
  */
QString PrintComposer::parseContacts(QSqlQueryModel *entityContactsModel, DBaseCtrl *dbCtrl)
{
    QString contactsHtml;


    if(entityContactsModel->rowCount() > 0)
    {
        contactsHtml = "<table width='400'>";
        contactsHtml += "<tr><td colspan=2 align='center' "
                       "style='background-color:lightgrey;color:black'>Personal Contacts Details</td></tr>";

        int personID;
        QSqlRecord personRecord;

        for(int row = 0; row < entityContactsModel->rowCount(); row++)
        {
           personID  = entityContactsModel->record(row).value(0).toInt();
           personRecord = dbCtrl->getSinglePersonModel(personID)->record(0);

           if(row > 0)
               contactsHtml += "<hr />";

           contactsHtml += PrintComposer::parsePerson(personRecord);

        }

        contactsHtml += "</table>";
    }
    else
        return "";

    return contactsHtml;
}

/**
  *
  */
QString PrintComposer::parsePerson(QSqlRecord personRecord)
{
    QString personHtml;
    QString fName, lName, homePH, officePH, mobilePH, fax, email, abn;


    fName = personRecord.value(DBaseCtrl::Person_FName).toString();
    lName = personRecord.value(DBaseCtrl::Person_LName).toString();
    officePH = personRecord.value(DBaseCtrl::Person_OfficePH).toString();
    homePH = personRecord.value(DBaseCtrl::Person_HomePH).toString();
    mobilePH = personRecord.value(DBaseCtrl::Person_MobilePH).toString();
    fax = personRecord.value(DBaseCtrl::Person_FAX).toString();
    email = personRecord.value(DBaseCtrl::Person_Email).toString();
    abn = personRecord.value(DBaseCtrl::Person_ABN).toString();

    personHtml += "<tr><td align='right' width='100' style='color:blue'>Person: </td><td>";
    personHtml += fName + " " + lName + "</td></tr>";

    if(!officePH.isEmpty())
    {
         personHtml += "<tr><td align='right' width='100' style='color:blue'>Office PH: </td><td>";
         personHtml += officePH + "</td></tr>";
    }

    if(!mobilePH.isEmpty())
    {
         personHtml += "<tr><td align='right' width='100' style='color:blue'>Mobile PH: </td><td>";
         personHtml += mobilePH + "</td></tr>";
    }

    if(!homePH.isEmpty())
    {
         personHtml += "<tr><td align='right' width='100' style='color:blue'>Home PH: </td><td>";
         personHtml += homePH + "</td></tr>";
    }

    if(!fax.isEmpty())
    {
         personHtml += "<tr><td align='right' width='100' style='color:blue'>FAX: </td><td>";
         personHtml += fax + "</td></tr>";
    }

    if(!email.isEmpty())
    {
         personHtml += "<tr><td align='right' width='100' style='color:blue'>Email: </td><td>";
         personHtml += email + "</td></tr>";
    }

    if(!abn.isEmpty())
    {
         personHtml += "<tr><td align='right' width='100' style='color:blue'>ABN: </td><td>";
         personHtml += abn + "</td></tr>";
    }

    return personHtml;
}


/**
  *
  */
QString PrintComposer::tableRow(QSqlRecord record, double *subTotal)
{
    QString tableRow;
    QString col1, col2;
    int col3;
    double col4, col5;

    col1 = record.value(DBaseCtrl::MoQ_Name).toString();
    col2 = record.value(DBaseCtrl::MoQ_Brand).toString();
    col3 = record.value(DBaseCtrl::MoQ_QTY).toInt();
    col4 = record.value(DBaseCtrl::MoQ_Price).toDouble();
    col5 = col3 * col4;

    tableRow += "<tr><td width='30%' style='color:blue'>" + col1 + "</td>"
                "<td width='30%' style='color:blue'>" + col2 + "</td>"
                "<td width='10%' align='right' style='color:blue'>" + QString::number(col3) + "</td>"
                "<td width='15%' align='right' style='color:blue'>$" + QString::number(col4, 'f', 2) + "</td>"
                "<td width='15%' align='right' style='color:orange'>$" + QString::number(col5, 'f', 2) + "</td></tr>";

    *subTotal = col5;
    return tableRow;
}

/**
  *
  */
QString PrintComposer::toAddress(int addressID, int entityID, int entityType, DBaseCtrl *dbCtrl)
{
    QStringList ownerDetails;
    QSqlRecord ownerRecord = dbCtrl->getOwnerModel()->record(0);

    quoteHeader = footerHtml(ownerRecord.value(DBaseCtrl::Owner_QuoteHeader).toString());
    quoteFooter = footerHtml(ownerRecord.value(DBaseCtrl::Owner_QuoteFooter).toString());
    invoiceHeader = footerHtml(ownerRecord.value(DBaseCtrl::Owner_InvoiceHeader).toString());
    invoiceFooter = footerHtml(ownerRecord.value(DBaseCtrl::Owner_InvoiceFooter).toString());


    ownerDetails << ownerRecord.value(DBaseCtrl::Owner_BusinessName).toString();

    QString tmpAddress = "";
    if(!ownerRecord.value(DBaseCtrl::Owner_UnitNO).toString().isEmpty())
        tmpAddress = ownerRecord.value(DBaseCtrl::Owner_UnitNO).toString() + " / ";

    if(!ownerRecord.value(DBaseCtrl::Owner_StreetNO).toString().isEmpty())
            tmpAddress += ownerRecord.value(DBaseCtrl::Owner_StreetNO).toString() + " ";

    if(!ownerRecord.value(DBaseCtrl::Owner_Street).toString().isEmpty())
        tmpAddress += ownerRecord.value(DBaseCtrl::Owner_Street).toString();

    ownerDetails << tmpAddress;

    if(!ownerRecord.value(DBaseCtrl::Owner_Suburb).toString().isEmpty())
        ownerDetails << ownerRecord.value(DBaseCtrl::Owner_Suburb).toString() + ", " +
                ownerRecord.value(DBaseCtrl::Owner_Postcode).toString();

    if(!ownerRecord.value(DBaseCtrl::Owner_City).toString().isEmpty())
        ownerDetails << ownerRecord.value(DBaseCtrl::Owner_City).toString();

    if(!ownerRecord.value(DBaseCtrl::Owner_State).toString().isEmpty())
        ownerDetails << ownerRecord.value(DBaseCtrl::Owner_State).toString();

    QStringList toDetails;

    if(entityType == 0)
        toDetails << dbCtrl->getSingleCompanyModel(entityID)->record(0).value(DBaseCtrl::Company_Name).toString();
    else
    {
        QSqlQueryModel *personAddress = dbCtrl->getSinglePersonModel(entityID);
        toDetails << personAddress->record(0).value(DBaseCtrl::Person_FName).toString() + " " +
                     personAddress->record(0).value(DBaseCtrl::Person_LName).toString();
    }

    QSqlRecord addressRecord = dbCtrl->getSingleAddressModel(addressID)->record(0);

    tmpAddress = "";
    if(!addressRecord.value(DBaseCtrl::Address_POBox).toString().isEmpty())
    {
        toDetails << "PO Box: " + addressRecord.value(DBaseCtrl::Address_POBox).toString();
    }
    else
    {
        if(!addressRecord.value(DBaseCtrl::Address_UnitNum).toString().isEmpty())
            tmpAddress = addressRecord.value(DBaseCtrl::Address_UnitNum).toString() + " / ";

        if(!addressRecord.value(DBaseCtrl::Address_HouseNum).toString().isEmpty())
            tmpAddress += addressRecord.value(DBaseCtrl::Address_HouseNum).toString() + " ";

        if(!addressRecord.value(DBaseCtrl::Address_Street).toString().isEmpty())
            tmpAddress += addressRecord.value(DBaseCtrl::Address_Street).toString();

        toDetails << tmpAddress;

    }
    tmpAddress = "";

    if(!addressRecord.value(DBaseCtrl::Address_Suburb).toString().isEmpty())
        tmpAddress = addressRecord.value(DBaseCtrl::Address_Suburb).toString() + ", ";

    if(!addressRecord.value(DBaseCtrl::Address_Postcode).toString().isEmpty())
        tmpAddress += addressRecord.value(DBaseCtrl::Address_Postcode).toString();

    toDetails << tmpAddress;

    toDetails << addressRecord.value(DBaseCtrl::Address_City).toString();

    int stateID = addressRecord.value(DBaseCtrl::Address_State).toInt();

    QSqlTableModel *stateModel = dbCtrl->getStateModel();
    for(int i = 0; i < stateModel->rowCount(); i++)
            if(stateModel->record(i).value(DBaseCtrl::State_ID).toInt() == stateID)
                 toDetails << stateModel->record(i).value(DBaseCtrl::State_ShortName).toString();

    int count = ownerDetails.count();
    if(toDetails.count() > count)
        count = toDetails.count();

    QString html, toTmp, ownerTmp;
    for (int row = 0; row < count; row++)
    {
        if(row < toDetails.count())
            toTmp = toDetails[row];
        else
            toTmp = "";

        if(row < ownerDetails.count())
            ownerTmp = ownerDetails[row];
        else
            ownerTmp = "";

        html += "<tr><td align='left' width='200' style='color:blue'>" + toTmp + "</td>"
                "<td></td>"
                "<td align='left' width='200' style='color:blue'>" + ownerTmp + "</td>";
    }

    return html;
}

QString PrintComposer::totalToHtml(double total)
{
    // GST + Total table
    QString html;

    int gst = (total / 11.0) * 100;
    double transGST = gst / 100.00;

    html += "<table width='30%' align='right'>";
    html += "<tr><td width='50%' align='right' style='color:blue'>GST: </td>"
            "<td width='50%' align='right' style='background-color:lightgrey;color:black;border:solid'>$" +
            QString::number(transGST, 'f', 2) + "</td>";

    html += "<tr><td width='50%' align='right' style='color:blue'>Total: </td>"
            "<td width='50%' align='right' style='background-color:lightgrey;color:black;border:solid'>$" +
            QString::number(total, 'f', 2) + "</td>";

    html += "</table>";

    return html;
}

QString PrintComposer::dateHeader(QString entityName, QString entityNumber, QString entityDate, QString entityDueDate)
{
    QString html;

    html = "<table width='100%'>";
    html += "<tr><td align='left' width='33%'><h4>" + entityName + ": " + entityNumber + "</h4></td>"
            "<td width='33%'></td>"
            "<td align='left' width='33%'><h4>Date : " +
            QDate::fromString(entityDate, "yyyyMMdd").toString("dd-MMM-yyyy") +
            "</h4></td></tr>";

    if(entityName == "Invoice")
        html += "<tr><td width='33%'></td>"
                "<td width='33%'></td>"
                "<td align='left' width='33%'><h4>Due Date : " +
                QDate::fromString(entityDueDate, "yyyyMMdd").toString("dd-MMM-yyyy") +
                "</h4></td></tr>";

    html += "</table>";

    return html;
}

QString PrintComposer::descriptionHtml(QString description)
{
    QString html = "<h4>Description: </h4>";
    html += "<p>" + description.replace("\n", "<br />") + "</p>";

    return html;
}

QString PrintComposer::basTable(double totalSales, double totalPurchases,
                                double gstOnSales, double gstOnPurchases,
                                double paymentRefund, QString returnPaymentColour,
                                QString paymentOrRefund)
{
    QString html;

    html += "<table width='90%' align='center'>";

    html += "<tr><td colspan='2' width='100%'"
            "style='background-color:lightgrey'"
            "align='center'>Goods and Services Tax (GST)</td></tr>";

    html += "<tr><td align='right' width='50%'>Total Sales (G1): </td><td align='left'>$" +
            QString::number(totalSales, 'f', 2) + "</td></tr>";

    html += "<tr><td align='right' width='50%'>Non-Capital Purchases (G11): </td><td align='left'>$" +
            QString::number(totalPurchases, 'f', 2) + "</td></tr>";


    html += "<tr></tr><tr><td colspan='2' width='100%'"
            "style='background-color:lightgrey'"
            "align='center'>Summary</td></tr>";

    html += "<tr><td align='right' width='50%'>GST on Sales (1A): </td><td align='left'>$" +
            QString::number(gstOnSales, 'f', 2) + "</td></tr>";

    html += "<tr><td align='right' width='50%'>GST on Purchases (1B): </td><td align='left'>$" +
            QString::number(gstOnPurchases, 'f', 2) +"</td></tr>";


    html += "<tr></tr><tr><td colspan='2' width='100%'"
            "style='background-color:lightgrey'"
            "align='center'>Payment or Refund?</td></tr>";

    html += "<tr><td align='right' width='50%' " + returnPaymentColour +
            " ><h3>" + paymentOrRefund +
            "</h3></td><td align='left' " + returnPaymentColour + " ><h3>$" +
            QString::number(paymentRefund, 'f', 0) + "</h3></td></tr>";

    html += endTableHtml;



    return html;
}

QString PrintComposer::basBreakdownTable(QString name, QString qty, QString amount, double gst)
{
    QString html = "<tr>"
                   "<td>" + name + "</td>"
                   "<td>" + qty + "</td>"
                   "<td>" + amount + "</td>"
                   "<td>" + QString::number((gst / 11), 'f', 2) +
                   "</td></tr>";

    return html;
}

QString PrintComposer::pandlTotals(double total, double gstTotal)
{
    QString html = "<tr></tr>"
                   "<tr>"
                   "<td colspan='2' align='right'>Totals: </td>"
                   "<td>" + QString::number(total, 'f', 2) + "</td>"
                   "<td>" + QString::number((gstTotal / 11), 'f', 2) + "</td>"
                   "</tr>";

    return html;
}

QString PrintComposer::abnHtml(QString abn)
{
    QString html = "<center><h4>ABN: " + abn + "</h4></center>";
    return html;
}
