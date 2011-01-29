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

#define X_OFFSET 25
#define Y_OFFSET 200

#define QUOTE_Y_OFFSET 0
#define QUOTE_PAPER_Y_OFFSET 15
#define INVOICE_Y_OFFSET 50
#define RECEIPTS_Y_OFFSET 100
#define EXPENSES_Y_OFFSET 150
#define REPORTS_Y_OFFSET 200
#define PAPER_X_OFFSET 38

#define DRAWER_OPEN_OFFSET 30
#define DRAWER_FRONT_OFFSET 23

#define PLATE_X_OFFSET 37

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imgobject.h"

#include <QtGui>

void MainWindow::initObjects()
{
    ui->navPanel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    scene = new QGraphicsScene(QRect(0, 0,
                                          ui->navContainer->size().width(),
                                          ui->navContainer->size().height())
                                               ,ui->navContainer);
    scene->setBackgroundBrush(scene->palette().window());
    ui->navPanel->setScene(scene);

    initPaper();
    initDrawers();
    initDropMenus();
}

void MainWindow::initPaper()
{
    paperItem = new ImgObject(":/icons/paper.png");

    scene->addItem(paperItem);

    paperItem->setZValue(98);

    paperMachine = new QStateMachine(this);
    paperStates[quotes] = new QState(paperMachine);
    paperStates[invoices] = new QState(paperMachine);
    paperStates[receipts] = new QState(paperMachine);
    paperStates[expenses] = new QState(paperMachine);
    paperStates[reports] = new QState(paperMachine);
    paperStates[5] = new QState(paperMachine);

    QGraphicsRotation *paperRotation = new QGraphicsRotation(paperItem);
    paperRotation->setAxis(Qt::XAxis);
    paperItem->setTransformations(QList<QGraphicsTransform *>() << paperRotation);


    paperStates[quotes]->assignProperty(paperItem, "pos", QPointF(PAPER_X_OFFSET, Y_OFFSET + QUOTE_PAPER_Y_OFFSET));
    paperStates[quotes]->assignProperty(paperRotation, "angle", qVariantValue<qreal>(65));

    paperStates[invoices]->assignProperty(paperItem, "pos", QPointF(PAPER_X_OFFSET, Y_OFFSET + INVOICE_Y_OFFSET + QUOTE_PAPER_Y_OFFSET));
    paperStates[invoices]->assignProperty(paperRotation, "angle", qVariantValue<qreal>(75));

    paperStates[receipts]->assignProperty(paperItem, "pos", QPointF(PAPER_X_OFFSET, Y_OFFSET + RECEIPTS_Y_OFFSET + QUOTE_PAPER_Y_OFFSET));
    paperStates[receipts]->assignProperty(paperRotation, "angle", qVariantValue<qreal>(68));

    paperStates[expenses]->assignProperty(paperItem, "pos", QPointF(PAPER_X_OFFSET, Y_OFFSET + EXPENSES_Y_OFFSET + QUOTE_PAPER_Y_OFFSET));
    paperStates[expenses]->assignProperty(paperRotation, "angle", qVariantValue<qreal>(68));

    paperStates[reports]->assignProperty(paperItem, "pos", QPointF(PAPER_X_OFFSET, Y_OFFSET + REPORTS_Y_OFFSET + QUOTE_PAPER_Y_OFFSET));
    paperStates[reports]->assignProperty(paperRotation, "angle", qVariantValue<qreal>(68));

    paperStates[5]->assignProperty(paperItem, "pos", QPointF(PAPER_X_OFFSET, Y_OFFSET + 300));
    paperStates[5]->assignProperty(paperRotation, "angle", qVariantValue<qreal>(0));

    paperStates[5]->addTransition(this, SIGNAL(quotePaperPos()), paperStates[quotes]);
    paperTrans[quotes] = paperStates[5]->addTransition(this, SIGNAL(paperToQuote()), paperStates[quotes]);
    QPropertyAnimation *quotePaperAnim = new QPropertyAnimation(paperItem, "pos");
    quotePaperAnim->setDuration(1000);
    paperTrans[quotes]->addAnimation(quotePaperAnim);
    QPropertyAnimation *quotePaperAngle = new QPropertyAnimation(paperRotation, "angle");
    quotePaperAngle->setDuration(1000);
    paperTrans[quotes]->addAnimation(quotePaperAngle);
    connect(paperTrans[quotes]->animations().back(), SIGNAL(finished()), this, SLOT(quotePaperHome()));

    paperStates[5]->addTransition(this, SIGNAL(invoicePaperPos()), paperStates[invoices]);
    paperTrans[invoices] = paperStates[5]->addTransition(this, SIGNAL(paperToInvoice()), paperStates[invoices]);
    QPropertyAnimation *invoicePaperAnim = new QPropertyAnimation(paperItem, "pos");
    invoicePaperAnim->setDuration(1000);
    paperTrans[invoices]->addAnimation(invoicePaperAnim);
    QPropertyAnimation *invoicePaperAngle = new QPropertyAnimation(paperRotation, "angle");
    invoicePaperAngle->setDuration(1000);
    paperTrans[invoices]->addAnimation(invoicePaperAngle);
    connect(paperTrans[invoices]->animations().back(), SIGNAL(finished()), this, SLOT(closeDrawers()));

    paperStates[5]->addTransition(this, SIGNAL(receiptPaperPos()), paperStates[receipts]);
    paperTrans[receipts] = paperStates[5]->addTransition(this, SIGNAL(paperToReceipt()), paperStates[receipts]);
    QPropertyAnimation *receiptPaperAnim = new QPropertyAnimation(paperItem, "pos");
    receiptPaperAnim->setDuration(1000);
    paperTrans[receipts]->addAnimation(receiptPaperAnim);
    QPropertyAnimation *receiptPaperAngle = new QPropertyAnimation(paperRotation, "angle");
    receiptPaperAngle->setDuration(1000);
    paperTrans[receipts]->addAnimation(receiptPaperAngle);
    connect(paperTrans[receipts]->animations().back(), SIGNAL(finished()), this, SLOT(closeDrawers()));

    paperStates[5]->addTransition(this, SIGNAL(expensePaperPos()), paperStates[expenses]);
    paperTrans[expenses] = paperStates[5]->addTransition(this, SIGNAL(paperToExpense()), paperStates[expenses]);
    QPropertyAnimation *expensePaperAnim = new QPropertyAnimation(paperItem, "pos");
    expensePaperAnim->setDuration(1000);
    paperTrans[expenses]->addAnimation(expensePaperAnim);
    QPropertyAnimation *expensePaperAngle = new QPropertyAnimation(paperRotation, "angle");
    expensePaperAngle->setDuration(1000);
    paperTrans[expenses]->addAnimation(expensePaperAngle);
    connect(paperTrans[expenses]->animations().back(), SIGNAL(finished()), this, SLOT(closeDrawers()));

    paperStates[5]->addTransition(this, SIGNAL(reportPaperPos()), paperStates[reports]);
    paperTrans[reports] = paperStates[5]->addTransition(this, SIGNAL(paperToReport()), paperStates[reports]);
    QPropertyAnimation *reportPaperAnim = new QPropertyAnimation(paperItem, "pos");
    reportPaperAnim->setDuration(1000);
    paperTrans[reports]->addAnimation(reportPaperAnim);
    QPropertyAnimation *reportPaperAngle = new QPropertyAnimation(paperRotation, "angle");
    reportPaperAngle->setDuration(1000);
    paperTrans[reports]->addAnimation(reportPaperAngle);
    connect(paperTrans[reports]->animations().back(), SIGNAL(finished()), this, SLOT(closeDrawers()));

    paperTrans[5] = paperStates[quotes]->addTransition(this, SIGNAL(homePos()), paperStates[5]);
    paperTrans[5] = paperStates[quotes]->addTransition(this, SIGNAL(quotePaperHomePos()), paperStates[5]);
    paperTrans[5] = paperStates[invoices]->addTransition(this, SIGNAL(homePos()), paperStates[5]);
    paperTrans[5] = paperStates[invoices]->addTransition(this, SIGNAL(drawerHome()), paperStates[5]);
    paperTrans[5] = paperStates[receipts]->addTransition(this, SIGNAL(homePos()), paperStates[5]);
    paperTrans[5] = paperStates[receipts]->addTransition(this, SIGNAL(drawerHome()), paperStates[5]);
    paperTrans[5] = paperStates[expenses]->addTransition(this, SIGNAL(homePos()), paperStates[5]);
    paperTrans[5] = paperStates[expenses]->addTransition(this, SIGNAL(drawerHome()), paperStates[5]);
    paperTrans[5] = paperStates[reports]->addTransition(this, SIGNAL(homePos()), paperStates[5]);
    paperTrans[5] = paperStates[reports]->addTransition(this, SIGNAL(drawerHome()), paperStates[5]);

    paperTrans[quotes + 6] = paperStates[quotes]->addTransition(this, SIGNAL(paperFromQuote()), paperStates[5]);
    QPropertyAnimation *fromQuoteAnim = new QPropertyAnimation(paperItem, "pos");
    fromQuoteAnim->setDuration(1000);
    fromQuoteAnim->setEasingCurve(QEasingCurve::InQuint);
    paperTrans[quotes + 6]->addAnimation(fromQuoteAnim);
    QPropertyAnimation *fromQuoteAngle = new QPropertyAnimation(paperRotation, "angle");
    fromQuoteAngle->setDuration(1000);
    paperTrans[quotes + 6]->addAnimation(fromQuoteAngle);
    connect(paperTrans[quotes + 6]->animations().back(), SIGNAL(finished()), this, SLOT(openNewQuote()));

    paperTrans[invoices + 6] = paperStates[invoices]->addTransition(this, SIGNAL(paperFromInvoice()), paperStates[5]);
    QPropertyAnimation *fromInvoiceAnim = new QPropertyAnimation(paperItem, "pos");
    fromInvoiceAnim->setDuration(1000);
    fromInvoiceAnim->setEasingCurve(QEasingCurve::InQuint);
    paperTrans[invoices + 6]->addAnimation(fromInvoiceAnim);
    QPropertyAnimation *fromInvoiceAngle = new QPropertyAnimation(paperRotation, "angle");
    fromInvoiceAngle->setDuration(1000);
    paperTrans[invoices + 6]->addAnimation(fromInvoiceAngle);
    connect(paperTrans[invoices + 6]->animations().back(), SIGNAL(finished()), this, SLOT(openNewInvoice()));

    paperTrans[receipts + 6] = paperStates[receipts]->addTransition(this, SIGNAL(paperFromReceipt()), paperStates[5]);
    QPropertyAnimation *fromReceiptAnim = new QPropertyAnimation(paperItem, "pos");
    fromReceiptAnim->setDuration(1000);
    fromReceiptAnim->setEasingCurve(QEasingCurve::InQuint);
    paperTrans[receipts + 6]->addAnimation(fromReceiptAnim);
    QPropertyAnimation *fromReceiptAngle = new QPropertyAnimation(paperRotation, "angle");
    fromReceiptAngle->setDuration(1000);
    paperTrans[receipts + 6]->addAnimation(fromReceiptAngle);
    connect(paperTrans[receipts + 6]->animations().back(), SIGNAL(finished()), this, SLOT(openNewReceipt()));

    paperTrans[expenses + 6] = paperStates[expenses]->addTransition(this, SIGNAL(paperFromExpense()), paperStates[5]);
    QPropertyAnimation *fromExpensesAnim = new QPropertyAnimation(paperItem, "pos");
    fromExpensesAnim->setDuration(1000);
    fromExpensesAnim->setEasingCurve(QEasingCurve::InQuint);
    paperTrans[expenses + 6]->addAnimation(fromExpensesAnim);
    QPropertyAnimation *fromExpensesAngle = new QPropertyAnimation(paperRotation, "angle");
    fromExpensesAngle->setDuration(1000);
    paperTrans[expenses + 6]->addAnimation(fromExpensesAngle);
    connect(paperTrans[expenses + 6]->animations().back(), SIGNAL(finished()), this, SLOT(openNewExpense()));

    paperTrans[reports + 6] = paperStates[reports]->addTransition(this, SIGNAL(paperFromReport()), paperStates[5]);
    QPropertyAnimation *fromReportsAnim = new QPropertyAnimation(paperItem, "pos");
    fromReportsAnim->setDuration(1000);
    fromReportsAnim->setEasingCurve(QEasingCurve::InQuint);
    paperTrans[reports + 6]->addAnimation(fromReportsAnim);
    QPropertyAnimation *fromReportsAngle = new QPropertyAnimation(paperRotation, "angle");
    fromReportsAngle->setDuration(1000);
    paperTrans[reports + 6]->addAnimation(fromReportsAngle);
    connect(paperTrans[reports + 6]->animations().back(), SIGNAL(finished()), this, SLOT(openNewReport()));

    paperMachine->setInitialState(paperStates[5]);
    paperMachine->start();
}

void MainWindow::initDrawers()
{
    paperDrawers[quotes] = new ImgObject(":/icons/draw.png");
    paperDrawers[invoices] = new ImgObject(":/icons/draw.png");
    paperDrawers[receipts] = new ImgObject(":/icons/draw.png");
    paperDrawers[expenses] = new ImgObject(":/icons/draw.png");
    paperDrawers[reports] = new ImgObject(":/icons/draw.png");

    drawerFronts[quotes] = new DrawerFront(":/icons/drawerFront.png", "Quotes");
    drawerFronts[invoices] = new DrawerFront(":/icons/drawerFront.png", "Invoices");
    drawerFronts[receipts] = new DrawerFront(":/icons/drawerFront.png", "Receipts");
    drawerFronts[expenses] = new DrawerFront(":/icons/drawerFront.png", "Expenses");
    drawerFronts[reports] = new DrawerFront(":/icons/drawerFront.png", "Reports");

    scene->addItem(paperDrawers[reports]);
    scene->addItem(paperDrawers[expenses]);
    scene->addItem(paperDrawers[receipts]);
    scene->addItem(paperDrawers[invoices]);
    scene->addItem(paperDrawers[quotes]);

    scene->addItem(drawerFronts[reports]);
    scene->addItem(drawerFronts[expenses]);
    scene->addItem(drawerFronts[receipts]);
    scene->addItem(drawerFronts[invoices]);
    scene->addItem(drawerFronts[quotes]);

    paperDrawers[reports]->setZValue(71);
    drawerFronts[reports]->setZValue(90);
    paperDrawers[expenses]->setZValue(72);
    drawerFronts[expenses]->setZValue(90);
    paperDrawers[receipts]->setZValue(73);
    drawerFronts[receipts]->setZValue(90);
    paperDrawers[invoices]->setZValue(74);
    drawerFronts[invoices]->setZValue(90);
    paperDrawers[quotes]->setZValue(75);
    drawerFronts[quotes]->setZValue(90);


    machine = new QStateMachine;

    drawStates[quotes] = new QState(machine);
    drawStates[invoices] = new QState(machine);
    drawStates[receipts] = new QState(machine);
    drawStates[expenses] = new QState(machine);
    drawStates[reports] = new QState(machine);

    machine->setInitialState(drawStates[quotes]);

    // State home
    drawStates[quotes]->assignProperty(paperDrawers[quotes], "pos", QPointF(X_OFFSET, Y_OFFSET + QUOTE_Y_OFFSET));
    drawStates[quotes]->assignProperty(drawerFronts[quotes], "pos", QPointF(X_OFFSET, Y_OFFSET + QUOTE_Y_OFFSET + DRAWER_FRONT_OFFSET));
    drawStates[quotes]->assignProperty(paperDrawers[invoices], "pos", QPointF(X_OFFSET, Y_OFFSET + INVOICE_Y_OFFSET));
    drawStates[quotes]->assignProperty(drawerFronts[invoices], "pos", QPointF(X_OFFSET, Y_OFFSET + INVOICE_Y_OFFSET + DRAWER_FRONT_OFFSET));
    drawStates[quotes]->assignProperty(paperDrawers[receipts], "pos", QPointF(X_OFFSET, Y_OFFSET + RECEIPTS_Y_OFFSET ));
    drawStates[quotes]->assignProperty(drawerFronts[receipts], "pos", QPointF(X_OFFSET, Y_OFFSET + RECEIPTS_Y_OFFSET + DRAWER_FRONT_OFFSET));
    drawStates[quotes]->assignProperty(paperDrawers[expenses], "pos", QPointF(X_OFFSET, Y_OFFSET + EXPENSES_Y_OFFSET));
    drawStates[quotes]->assignProperty(drawerFronts[expenses], "pos", QPointF(X_OFFSET, Y_OFFSET + EXPENSES_Y_OFFSET + DRAWER_FRONT_OFFSET));
    drawStates[quotes]->assignProperty(paperDrawers[reports], "pos", QPointF(X_OFFSET, Y_OFFSET + REPORTS_Y_OFFSET));
    drawStates[quotes]->assignProperty(drawerFronts[reports], "pos", QPointF(X_OFFSET, Y_OFFSET + REPORTS_Y_OFFSET + DRAWER_FRONT_OFFSET));

    //State Invoice Just Move Quote Up
    drawStates[invoices]->assignProperty(paperDrawers[quotes], "pos", QPointF(X_OFFSET,  Y_OFFSET + QUOTE_Y_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[invoices]->assignProperty(drawerFronts[quotes], "pos", QPointF(X_OFFSET, Y_OFFSET + QUOTE_Y_OFFSET + DRAWER_FRONT_OFFSET - DRAWER_OPEN_OFFSET));

    drawStates[invoices]->assignProperty(paperDrawers[invoices], "pos", QPointF(X_OFFSET, Y_OFFSET + INVOICE_Y_OFFSET));
    drawStates[invoices]->assignProperty(drawerFronts[invoices], "pos", QPointF(X_OFFSET, Y_OFFSET + INVOICE_Y_OFFSET + DRAWER_FRONT_OFFSET));
    drawStates[invoices]->assignProperty(paperDrawers[receipts], "pos", QPointF(X_OFFSET, Y_OFFSET + RECEIPTS_Y_OFFSET));
    drawStates[invoices]->assignProperty(drawerFronts[receipts], "pos", QPointF(X_OFFSET, Y_OFFSET + RECEIPTS_Y_OFFSET + DRAWER_FRONT_OFFSET));
    drawStates[invoices]->assignProperty(paperDrawers[expenses], "pos", QPointF(X_OFFSET, Y_OFFSET + EXPENSES_Y_OFFSET));
    drawStates[invoices]->assignProperty(drawerFronts[expenses], "pos", QPointF(X_OFFSET, Y_OFFSET + EXPENSES_Y_OFFSET + DRAWER_FRONT_OFFSET));
    drawStates[invoices]->assignProperty(paperDrawers[reports], "pos", QPointF(X_OFFSET, Y_OFFSET + REPORTS_Y_OFFSET));
    drawStates[invoices]->assignProperty(drawerFronts[reports], "pos", QPointF(X_OFFSET, Y_OFFSET + REPORTS_Y_OFFSET + DRAWER_FRONT_OFFSET));

    //State Receipt Move Quote + Invoice
    drawStates[receipts]->assignProperty(paperDrawers[quotes], "pos", QPointF(X_OFFSET, Y_OFFSET + QUOTE_Y_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[receipts]->assignProperty(drawerFronts[quotes], "pos", QPointF(X_OFFSET, Y_OFFSET + QUOTE_Y_OFFSET + DRAWER_FRONT_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[receipts]->assignProperty(paperDrawers[invoices], "pos", QPointF(X_OFFSET, Y_OFFSET + INVOICE_Y_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[receipts]->assignProperty(drawerFronts[invoices], "pos", QPointF(X_OFFSET, Y_OFFSET + INVOICE_Y_OFFSET + DRAWER_FRONT_OFFSET - DRAWER_OPEN_OFFSET));

    drawStates[receipts]->assignProperty(paperDrawers[receipts], "pos", QPointF(X_OFFSET, Y_OFFSET + RECEIPTS_Y_OFFSET));
    drawStates[receipts]->assignProperty(drawerFronts[receipts], "pos", QPointF(X_OFFSET, Y_OFFSET + RECEIPTS_Y_OFFSET + DRAWER_FRONT_OFFSET));
    drawStates[receipts]->assignProperty(paperDrawers[expenses], "pos", QPointF(X_OFFSET, Y_OFFSET + EXPENSES_Y_OFFSET));
    drawStates[receipts]->assignProperty(drawerFronts[expenses], "pos", QPointF(X_OFFSET, Y_OFFSET + EXPENSES_Y_OFFSET + DRAWER_FRONT_OFFSET));
    drawStates[receipts]->assignProperty(paperDrawers[reports], "pos", QPointF(X_OFFSET, Y_OFFSET + REPORTS_Y_OFFSET));
    drawStates[receipts]->assignProperty(drawerFronts[reports], "pos", QPointF(X_OFFSET, Y_OFFSET + REPORTS_Y_OFFSET + DRAWER_FRONT_OFFSET));

    //State Expense Move Quote + Receipt
    drawStates[expenses]->assignProperty(paperDrawers[quotes], "pos", QPointF(X_OFFSET, Y_OFFSET + QUOTE_Y_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[expenses]->assignProperty(drawerFronts[quotes], "pos", QPointF(X_OFFSET, Y_OFFSET + QUOTE_Y_OFFSET + DRAWER_FRONT_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[expenses]->assignProperty(paperDrawers[invoices], "pos", QPointF(X_OFFSET, Y_OFFSET + INVOICE_Y_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[expenses]->assignProperty(drawerFronts[invoices], "pos", QPointF(X_OFFSET, Y_OFFSET + INVOICE_Y_OFFSET + DRAWER_FRONT_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[expenses]->assignProperty(paperDrawers[receipts], "pos", QPointF(X_OFFSET, Y_OFFSET + RECEIPTS_Y_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[expenses]->assignProperty(drawerFronts[receipts], "pos", QPointF(X_OFFSET, Y_OFFSET + RECEIPTS_Y_OFFSET + DRAWER_FRONT_OFFSET - DRAWER_OPEN_OFFSET));

    drawStates[expenses]->assignProperty(paperDrawers[expenses], "pos", QPointF(X_OFFSET, Y_OFFSET + EXPENSES_Y_OFFSET));
    drawStates[expenses]->assignProperty(drawerFronts[expenses], "pos", QPointF(X_OFFSET, Y_OFFSET + EXPENSES_Y_OFFSET + DRAWER_FRONT_OFFSET));
    drawStates[expenses]->assignProperty(paperDrawers[reports], "pos", QPointF(X_OFFSET, Y_OFFSET + REPORTS_Y_OFFSET));
    drawStates[expenses]->assignProperty(drawerFronts[reports], "pos", QPointF(X_OFFSET, Y_OFFSET + REPORTS_Y_OFFSET + DRAWER_FRONT_OFFSET));

    //State Report Move Quote + Inv + Recp + Expen
    drawStates[reports]->assignProperty(paperDrawers[quotes], "pos", QPointF(X_OFFSET, Y_OFFSET + QUOTE_Y_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[reports]->assignProperty(drawerFronts[quotes], "pos", QPointF(X_OFFSET, Y_OFFSET + QUOTE_Y_OFFSET + DRAWER_FRONT_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[reports]->assignProperty(paperDrawers[invoices], "pos", QPointF(X_OFFSET, Y_OFFSET + INVOICE_Y_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[reports]->assignProperty(drawerFronts[invoices], "pos", QPointF(X_OFFSET, Y_OFFSET + INVOICE_Y_OFFSET + DRAWER_FRONT_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[reports]->assignProperty(paperDrawers[receipts], "pos", QPointF(X_OFFSET, Y_OFFSET + RECEIPTS_Y_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[reports]->assignProperty(drawerFronts[receipts], "pos", QPointF(X_OFFSET, Y_OFFSET + RECEIPTS_Y_OFFSET + DRAWER_FRONT_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[reports]->assignProperty(paperDrawers[expenses], "pos", QPointF(X_OFFSET, Y_OFFSET + EXPENSES_Y_OFFSET - DRAWER_OPEN_OFFSET));
    drawStates[reports]->assignProperty(drawerFronts[expenses], "pos", QPointF(X_OFFSET, Y_OFFSET + EXPENSES_Y_OFFSET + DRAWER_FRONT_OFFSET - DRAWER_OPEN_OFFSET));

    drawStates[reports]->assignProperty(paperDrawers[reports], "pos", QPointF(X_OFFSET, Y_OFFSET + REPORTS_Y_OFFSET));
    drawStates[reports]->assignProperty(drawerFronts[reports], "pos", QPointF(X_OFFSET, Y_OFFSET + REPORTS_Y_OFFSET + DRAWER_FRONT_OFFSET));

    drawerTrans[invoiceHome] = drawStates[invoices]->addTransition(this, SIGNAL(homePos()), drawStates[quotes]);
    QPropertyAnimation *tmpInvoiceAnim = new QPropertyAnimation(paperDrawers[quotes], "pos");
    tmpInvoiceAnim->setDuration(1000);
    drawerTrans[invoiceHome]->addAnimation(tmpInvoiceAnim);
    tmpInvoiceAnim = new QPropertyAnimation(drawerFronts[quotes], "pos");
    tmpInvoiceAnim->setDuration(1000);
    drawerTrans[invoiceHome]->addAnimation(tmpInvoiceAnim);
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(paperDrawers[invoices], "pos"));
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(drawerFronts[invoices], "pos"));
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(paperDrawers[receipts], "pos"));
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(drawerFronts[receipts], "pos"));
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(paperDrawers[expenses], "pos"));
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(drawerFronts[expenses], "pos"));
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(paperDrawers[reports], "pos"));
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(drawerFronts[reports], "pos"));
    drawerTrans[invoiceHome] = drawStates[invoices]->addTransition(this, SIGNAL(drawerHome()), drawStates[quotes]);
    tmpInvoiceAnim = new QPropertyAnimation(paperDrawers[quotes], "pos");
    tmpInvoiceAnim->setDuration(1000);
    drawerTrans[invoiceHome]->addAnimation(tmpInvoiceAnim);
    tmpInvoiceAnim = new QPropertyAnimation(drawerFronts[quotes], "pos");
    tmpInvoiceAnim->setDuration(1000);
    drawerTrans[invoiceHome]->addAnimation(tmpInvoiceAnim);
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(paperDrawers[invoices], "pos"));
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(drawerFronts[invoices], "pos"));
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(paperDrawers[receipts], "pos"));
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(drawerFronts[receipts], "pos"));
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(paperDrawers[expenses], "pos"));
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(drawerFronts[expenses], "pos"));
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(paperDrawers[reports], "pos"));
    drawerTrans[invoiceHome]->addAnimation(new QPropertyAnimation(drawerFronts[reports], "pos"));


    drawerTrans[receiptHome] = drawStates[receipts]->addTransition(this, SIGNAL(homePos()), drawStates[quotes]);
    QPropertyAnimation *tmpReceiptAnim = new QPropertyAnimation(paperDrawers[quotes], "pos");
    tmpReceiptAnim->setDuration(1000);
    drawerTrans[receiptHome]->addAnimation(tmpReceiptAnim);
    tmpReceiptAnim = new QPropertyAnimation(drawerFronts[quotes], "pos");
    tmpReceiptAnim->setDuration(1000);
    drawerTrans[receiptHome]->addAnimation(tmpReceiptAnim);
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(paperDrawers[quotes], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(drawerFronts[quotes], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(paperDrawers[invoices], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(drawerFronts[invoices], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(paperDrawers[receipts], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(drawerFronts[receipts], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(paperDrawers[expenses], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(drawerFronts[expenses], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(paperDrawers[reports], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(drawerFronts[reports], "pos"));
    drawerTrans[receiptHome] = drawStates[receipts]->addTransition(this, SIGNAL(drawerHome()), drawStates[quotes]);
    tmpReceiptAnim = new QPropertyAnimation(paperDrawers[quotes], "pos");
    tmpReceiptAnim->setDuration(1000);
    drawerTrans[receiptHome]->addAnimation(tmpReceiptAnim);
    tmpReceiptAnim = new QPropertyAnimation(drawerFronts[quotes], "pos");
    tmpReceiptAnim->setDuration(1000);
    drawerTrans[receiptHome]->addAnimation(tmpReceiptAnim);
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(paperDrawers[quotes], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(drawerFronts[quotes], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(paperDrawers[invoices], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(drawerFronts[invoices], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(paperDrawers[receipts], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(drawerFronts[receipts], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(paperDrawers[expenses], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(drawerFronts[expenses], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(paperDrawers[reports], "pos"));
    drawerTrans[receiptHome]->addAnimation(new QPropertyAnimation(drawerFronts[reports], "pos"));

    drawerTrans[expenseHome] = drawStates[expenses]->addTransition(this, SIGNAL(homePos()), drawStates[quotes]);
    QPropertyAnimation *tmpExpenseAnim = new QPropertyAnimation(paperDrawers[quotes], "pos");
    tmpExpenseAnim->setDuration(1000);
    drawerTrans[expenseHome]->addAnimation(tmpExpenseAnim);
    tmpExpenseAnim = new QPropertyAnimation(drawerFronts[quotes], "pos");
    tmpExpenseAnim->setDuration(1000);
    drawerTrans[expenseHome]->addAnimation(tmpReceiptAnim);
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(paperDrawers[quotes], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(drawerFronts[quotes], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(paperDrawers[invoices], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(drawerFronts[invoices], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(paperDrawers[receipts], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(drawerFronts[receipts], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(paperDrawers[expenses], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(drawerFronts[expenses], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(paperDrawers[reports], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(drawerFronts[reports], "pos"));
    drawerTrans[expenseHome] = drawStates[expenses]->addTransition(this, SIGNAL(drawerHome()), drawStates[quotes]);
    tmpExpenseAnim = new QPropertyAnimation(paperDrawers[quotes], "pos");
    tmpExpenseAnim->setDuration(1000);
    drawerTrans[expenseHome]->addAnimation(tmpExpenseAnim);
    tmpExpenseAnim = new QPropertyAnimation(drawerFronts[quotes], "pos");
    tmpExpenseAnim->setDuration(1000);
    drawerTrans[expenseHome]->addAnimation(tmpReceiptAnim);
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(paperDrawers[quotes], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(drawerFronts[quotes], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(paperDrawers[invoices], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(drawerFronts[invoices], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(paperDrawers[receipts], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(drawerFronts[receipts], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(paperDrawers[expenses], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(drawerFronts[expenses], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(paperDrawers[reports], "pos"));
    drawerTrans[expenseHome]->addAnimation(new QPropertyAnimation(drawerFronts[reports], "pos"));

    drawerTrans[reportHome] = drawStates[reports]->addTransition(this, SIGNAL(homePos()), drawStates[quotes]);
    QPropertyAnimation *tmpReportAnim = new QPropertyAnimation(paperDrawers[quotes], "pos");
    tmpReportAnim->setDuration(1000);
    drawerTrans[expenseHome]->addAnimation(tmpReportAnim);
    tmpReportAnim = new QPropertyAnimation(drawerFronts[quotes], "pos");
    tmpReportAnim->setDuration(1000);
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(paperDrawers[quotes], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(drawerFronts[quotes], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(paperDrawers[invoices], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(drawerFronts[invoices], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(paperDrawers[receipts], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(drawerFronts[receipts], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(paperDrawers[expenses], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(drawerFronts[expenses], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(paperDrawers[reports], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(drawerFronts[reports], "pos"));
    drawerTrans[reportHome] = drawStates[reports]->addTransition(this, SIGNAL(drawerHome()), drawStates[quotes]);
    tmpReportAnim = new QPropertyAnimation(paperDrawers[quotes], "pos");
    tmpReportAnim->setDuration(1000);
    drawerTrans[expenseHome]->addAnimation(tmpReportAnim);
    tmpReportAnim = new QPropertyAnimation(drawerFronts[quotes], "pos");
    tmpReportAnim->setDuration(1000);
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(paperDrawers[quotes], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(drawerFronts[quotes], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(paperDrawers[invoices], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(drawerFronts[invoices], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(paperDrawers[receipts], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(drawerFronts[receipts], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(paperDrawers[expenses], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(drawerFronts[expenses], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(paperDrawers[reports], "pos"));
    drawerTrans[reportHome]->addAnimation(new QPropertyAnimation(drawerFronts[reports], "pos"));

    drawerTrans[invoices] = drawStates[quotes]->addTransition(this, SIGNAL(openInvoiceDraw()), drawStates[invoices]);
    drawerTrans[invoices]->addAnimation(new QPropertyAnimation(paperDrawers[quotes], "pos"));
    drawerTrans[invoices]->addAnimation(new QPropertyAnimation(drawerFronts[quotes], "pos"));
    drawerTrans[invoices]->addAnimation(new QPropertyAnimation(paperDrawers[invoices], "pos"));
    drawerTrans[invoices]->addAnimation(new QPropertyAnimation(drawerFronts[invoices], "pos"));
    drawerTrans[invoices]->addAnimation(new QPropertyAnimation(paperDrawers[receipts], "pos"));
    drawerTrans[invoices]->addAnimation(new QPropertyAnimation(drawerFronts[receipts], "pos"));
    drawerTrans[invoices]->addAnimation(new QPropertyAnimation(paperDrawers[expenses], "pos"));
    drawerTrans[invoices]->addAnimation(new QPropertyAnimation(drawerFronts[expenses], "pos"));
    drawerTrans[invoices]->addAnimation(new QPropertyAnimation(paperDrawers[reports], "pos"));
    drawerTrans[invoices]->addAnimation(new QPropertyAnimation(drawerFronts[reports], "pos"));

    drawerTrans[receipts] = drawStates[quotes]->addTransition(this, SIGNAL(openReceiptDraw()), drawStates[receipts]);
    drawerTrans[receipts]->addAnimation(new QPropertyAnimation(paperDrawers[quotes], "pos"));
    drawerTrans[receipts]->addAnimation(new QPropertyAnimation(drawerFronts[quotes], "pos"));
    drawerTrans[receipts]->addAnimation(new QPropertyAnimation(paperDrawers[invoices], "pos"));
    drawerTrans[receipts]->addAnimation(new QPropertyAnimation(drawerFronts[invoices], "pos"));
    drawerTrans[receipts]->addAnimation(new QPropertyAnimation(paperDrawers[receipts], "pos"));
    drawerTrans[receipts]->addAnimation(new QPropertyAnimation(drawerFronts[receipts], "pos"));
    drawerTrans[receipts]->addAnimation(new QPropertyAnimation(paperDrawers[expenses], "pos"));
    drawerTrans[receipts]->addAnimation(new QPropertyAnimation(drawerFronts[expenses], "pos"));
    drawerTrans[receipts]->addAnimation(new QPropertyAnimation(paperDrawers[reports], "pos"));
    drawerTrans[receipts]->addAnimation(new QPropertyAnimation(drawerFronts[reports], "pos"));

    drawerTrans[expenses] = drawStates[quotes]->addTransition(this, SIGNAL(openExpenseDraw()), drawStates[expenses]);
    drawerTrans[expenses]->addAnimation(new QPropertyAnimation(paperDrawers[quotes], "pos"));
    drawerTrans[expenses]->addAnimation(new QPropertyAnimation(drawerFronts[quotes], "pos"));
    drawerTrans[expenses]->addAnimation(new QPropertyAnimation(paperDrawers[invoices], "pos"));
    drawerTrans[expenses]->addAnimation(new QPropertyAnimation(drawerFronts[invoices], "pos"));
    drawerTrans[expenses]->addAnimation(new QPropertyAnimation(paperDrawers[receipts], "pos"));
    drawerTrans[expenses]->addAnimation(new QPropertyAnimation(drawerFronts[receipts], "pos"));
    drawerTrans[expenses]->addAnimation(new QPropertyAnimation(paperDrawers[expenses], "pos"));
    drawerTrans[expenses]->addAnimation(new QPropertyAnimation(drawerFronts[expenses], "pos"));
    drawerTrans[expenses]->addAnimation(new QPropertyAnimation(paperDrawers[reports], "pos"));
    drawerTrans[expenses]->addAnimation(new QPropertyAnimation(drawerFronts[reports], "pos"));

    drawerTrans[reports] = drawStates[quotes]->addTransition(this, SIGNAL(openReportDraw()), drawStates[reports]);
    drawerTrans[reports]->addAnimation(new QPropertyAnimation(paperDrawers[quotes], "pos"));
    drawerTrans[reports]->addAnimation(new QPropertyAnimation(drawerFronts[quotes], "pos"));
    drawerTrans[reports]->addAnimation(new QPropertyAnimation(paperDrawers[invoices], "pos"));
    drawerTrans[reports]->addAnimation(new QPropertyAnimation(drawerFronts[invoices], "pos"));
    drawerTrans[reports]->addAnimation(new QPropertyAnimation(paperDrawers[receipts], "pos"));
    drawerTrans[reports]->addAnimation(new QPropertyAnimation(drawerFronts[receipts], "pos"));
    drawerTrans[reports]->addAnimation(new QPropertyAnimation(paperDrawers[expenses], "pos"));
    drawerTrans[reports]->addAnimation(new QPropertyAnimation(drawerFronts[expenses], "pos"));
    drawerTrans[reports]->addAnimation(new QPropertyAnimation(paperDrawers[reports], "pos"));
    drawerTrans[reports]->addAnimation(new QPropertyAnimation(drawerFronts[reports], "pos"));


    machine->start();

    connect(paperDrawers[quotes], SIGNAL(clicked()), this, SLOT(setQuoteMenu()));
    connect(drawerFronts[quotes], SIGNAL(clicked()), this, SLOT(setQuoteMenu()));

    connect(paperDrawers[invoices], SIGNAL(clicked()), this, SLOT(setInvoiceMenu()));
    connect(drawerFronts[invoices], SIGNAL(clicked()), this, SLOT(setInvoiceMenu()));
    connect(drawerTrans[invoices]->animations().front(), SIGNAL(finished()), this, SLOT(invoiceDrawerClosed()));

    connect(paperDrawers[receipts], SIGNAL(clicked()), this, SLOT(setReceiptMenu()));
    connect(drawerFronts[receipts], SIGNAL(clicked()), this, SLOT(setReceiptMenu()));
    connect(drawerTrans[receipts]->animations().front(), SIGNAL(finished()), this, SLOT(receiptDrawerClosed()));

    connect(paperDrawers[expenses], SIGNAL(clicked()), this, SLOT(setExpenseMenu()));
    connect(drawerFronts[expenses], SIGNAL(clicked()), this, SLOT(setExpenseMenu()));
    connect(drawerTrans[expenses]->animations().front(), SIGNAL(finished()), this, SLOT(expenseDrawerClosed()));

    connect(paperDrawers[reports], SIGNAL(clicked()), this, SLOT(setReportMenu()));
    connect(drawerFronts[reports], SIGNAL(clicked()), this, SLOT(setReportMenu()));
    connect(drawerTrans[reports]->animations().front(), SIGNAL(finished()), this, SLOT(reportDrawerClosed()));
}

void MainWindow::initDropMenus()
{
    defaultMenuMachine = new QStateMachine;
    quoteMenuMachine = new QStateMachine;
    invoiceMenuMachine = new QStateMachine;
    receiptMenuMachine = new QStateMachine;
    expenseMenuMachine = new QStateMachine;
    reportMenuMachine = new QStateMachine;

    initDefaultMenu();
    initQuoteMenu();
    initInvoiceMenu();
    initReceiptMenu();
    initExpenseMenu();
    initReportMenu();

    defaultMenuMachine->start();
    quoteMenuMachine->start();
    invoiceMenuMachine->start();
    receiptMenuMachine->start();
    expenseMenuMachine->start();
    reportMenuMachine->start();
}

void MainWindow::initDefaultMenu()
{
    defaultMenuItem = new ImgObject(":/Logo");

    scene->addItem(defaultMenuItem);

    defaultMenuItem->setZValue(20);

    defaultMenuStates = new QState*[2];

    defaultMenuStates[CLOSED] = new QState(defaultMenuMachine);
    defaultMenuStates[OPEN] = new QState(defaultMenuMachine);

    defaultMenuMachine->setInitialState(defaultMenuStates[OPEN]);

    defaultMenuStates[OPEN]->assignProperty(defaultMenuItem, "pos", QPointF(15, -10));
    defaultMenuStates[CLOSED]->assignProperty(defaultMenuItem, "pos", QPointF(15, -200));

    defaultMenuTrans = new QAbstractTransition*[2];
    defaultMenuTrans[OPEN] = defaultMenuStates[CLOSED]->addTransition(this, SIGNAL(openDefaultMenu()),defaultMenuStates[OPEN]);
    defaultMenuTrans[OPEN]->addAnimation(new QPropertyAnimation(defaultMenuItem, "pos"));

    defaultMenuTrans[CLOSED] = defaultMenuStates[OPEN]->addTransition(this, SIGNAL(homePos()),defaultMenuStates[CLOSED]);
    defaultMenuTrans[CLOSED]->addAnimation(new QPropertyAnimation(defaultMenuItem, "pos"));

    connect(defaultMenuTrans[CLOSED]->animations().back(), SIGNAL(finished()), this, SLOT(lowerMenu()));
}

void MainWindow::initQuoteMenu()
{
    dropMenus[quotes][newQuote] = new ChainPanel(":/icons/chaintitle.png", "New");
    dropMenus[quotes][openQuote] = new ChainPanel(":/icons/chaintitle.png", "Find");
    dropMenus[quotes][convertToInvoice] = new ChainPanel(":/icons/chaintitle.png", "To Invoice");
    dropMenus[quotes][deleteQuote] = new ChainPanel(":/icons/chaintitle.png", "Delete");

    scene->addItem(dropMenus[quotes][deleteQuote]);
    scene->addItem(dropMenus[quotes][convertToInvoice]);
    scene->addItem(dropMenus[quotes][openQuote]);
    scene->addItem(dropMenus[quotes][newQuote]);

    dropMenus[quotes][newQuote]->setZValue(24);
    dropMenus[quotes][openQuote]->setZValue(23);
    dropMenus[quotes][convertToInvoice]->setZValue(22);
    dropMenus[quotes][deleteQuote]->setZValue(21);

    menuStates[quotes] = new QState*[2];

    menuStates[quotes][CLOSED] = new QState(quoteMenuMachine);
    menuStates[quotes][OPEN] = new QState(quoteMenuMachine);

    quoteMenuMachine->setInitialState(menuStates[quotes][CLOSED]);

    // State Quotes OPEN
    menuStates[quotes][OPEN]->assignProperty(dropMenus[quotes][newQuote], "pos", QPointF(PLATE_X_OFFSET, -25));
    menuStates[quotes][OPEN]->assignProperty(dropMenus[quotes][openQuote], "pos", QPointF(PLATE_X_OFFSET, 25));
    menuStates[quotes][OPEN]->assignProperty(dropMenus[quotes][convertToInvoice], "pos", QPointF(PLATE_X_OFFSET, 75));
    menuStates[quotes][OPEN]->assignProperty(dropMenus[quotes][deleteQuote], "pos", QPointF(PLATE_X_OFFSET, 125));

    // State Quotes CLOSED
    menuStates[quotes][CLOSED]->assignProperty(dropMenus[quotes][newQuote], "pos", QPointF(PLATE_X_OFFSET, -230));
    menuStates[quotes][CLOSED]->assignProperty(dropMenus[quotes][openQuote], "pos", QPointF(PLATE_X_OFFSET, -180));
    menuStates[quotes][CLOSED]->assignProperty(dropMenus[quotes][convertToInvoice], "pos", QPointF(PLATE_X_OFFSET, -130));
    menuStates[quotes][CLOSED]->assignProperty(dropMenus[quotes][deleteQuote], "pos", QPointF(PLATE_X_OFFSET, -80));

    menuTrans[quotes] = new QAbstractTransition*[2];

    menuTrans[quotes][OPEN] = menuStates[quotes][CLOSED]->addTransition(this, SIGNAL(openQuoteMenu()), menuStates[quotes][OPEN]);
    menuTrans[quotes][OPEN]->addAnimation(new QPropertyAnimation(dropMenus[quotes][newQuote], "pos"));
    menuTrans[quotes][OPEN]->addAnimation(new QPropertyAnimation(dropMenus[quotes][openQuote], "pos"));
    menuTrans[quotes][OPEN]->addAnimation(new QPropertyAnimation(dropMenus[quotes][convertToInvoice], "pos"));
    menuTrans[quotes][OPEN]->addAnimation(new QPropertyAnimation(dropMenus[quotes][deleteQuote], "pos"));

    menuTrans[quotes][CLOSED] = menuStates[quotes][OPEN]->addTransition(this, SIGNAL(homePos()), menuStates[quotes][CLOSED]);
    menuTrans[quotes][CLOSED]->addAnimation(new QPropertyAnimation(dropMenus[quotes][newQuote], "pos"));
    menuTrans[quotes][CLOSED]->addAnimation(new QPropertyAnimation(dropMenus[quotes][openQuote], "pos"));
    menuTrans[quotes][CLOSED]->addAnimation(new QPropertyAnimation(dropMenus[quotes][convertToInvoice], "pos"));
    menuTrans[quotes][CLOSED]->addAnimation(new QPropertyAnimation(dropMenus[quotes][deleteQuote], "pos"));

    connect(dropMenus[quotes][newQuote], SIGNAL(clicked()), this, SLOT(createNewQuote()));
    connect(dropMenus[quotes][openQuote], SIGNAL(clicked()), this, SLOT(openExistingQuote()));
    connect(dropMenus[quotes][convertToInvoice], SIGNAL(clicked()), this, SLOT(quoteToInvoice()));
    connect(dropMenus[quotes][deleteQuote], SIGNAL(clicked()), this, SLOT(quoteDelete()));

    connect(menuTrans[quotes][CLOSED]->animations().back(), SIGNAL(finished()), this, SLOT(lowerMenu()));
}

void MainWindow::initInvoiceMenu()
{
    dropMenus[invoices][newInvoice] = new ChainPanel(":/icons/chaintitle.png", "New");
    dropMenus[invoices][openAnInvoice] = new ChainPanel(":/icons/chaintitle.png", "Find");
    dropMenus[invoices][convertToReceipt] = new ChainPanel(":/icons/chaintitle.png", "To Receipt");
    dropMenus[invoices][overdueInvoices] = new ChainPanel(":/icons/chaintitle.png", "Overdue");

    scene->addItem(dropMenus[invoices][overdueInvoices]);
    scene->addItem(dropMenus[invoices][convertToReceipt]);
    scene->addItem(dropMenus[invoices][openAnInvoice]);
    scene->addItem(dropMenus[invoices][newInvoice]);

    dropMenus[invoices][newInvoice]->setZValue(24);
    dropMenus[invoices][openAnInvoice]->setZValue(23);
    dropMenus[invoices][convertToReceipt]->setZValue(22);
    dropMenus[invoices][overdueInvoices]->setZValue(21);

    menuStates[invoices] = new QState*[2];

    menuStates[invoices][CLOSED] = new QState(invoiceMenuMachine);
    menuStates[invoices][OPEN] = new QState(invoiceMenuMachine);

    invoiceMenuMachine->setInitialState(menuStates[invoices][CLOSED]);

    // State Invoices OPEN
    menuStates[invoices][OPEN]->assignProperty(dropMenus[invoices][newInvoice], "pos", QPointF(PLATE_X_OFFSET, -25));
    menuStates[invoices][OPEN]->assignProperty(dropMenus[invoices][openAnInvoice], "pos", QPointF(PLATE_X_OFFSET, 25));
    menuStates[invoices][OPEN]->assignProperty(dropMenus[invoices][convertToReceipt], "pos", QPointF(PLATE_X_OFFSET, 75));
    menuStates[invoices][OPEN]->assignProperty(dropMenus[invoices][overdueInvoices], "pos", QPointF(PLATE_X_OFFSET, 125));

    // State Invoices CLOSED
    menuStates[invoices][CLOSED]->assignProperty(dropMenus[invoices][newInvoice], "pos", QPointF(PLATE_X_OFFSET, -230));
    menuStates[invoices][CLOSED]->assignProperty(dropMenus[invoices][openAnInvoice], "pos", QPointF(PLATE_X_OFFSET, -180));
    menuStates[invoices][CLOSED]->assignProperty(dropMenus[invoices][convertToReceipt], "pos", QPointF(PLATE_X_OFFSET, -130));
    menuStates[invoices][CLOSED]->assignProperty(dropMenus[invoices][overdueInvoices], "pos", QPointF(PLATE_X_OFFSET, -80));

    menuTrans[invoices] = new QAbstractTransition*[2];

    menuTrans[invoices][OPEN] = menuStates[invoices][CLOSED]->addTransition(this, SIGNAL(openInvoice()), menuStates[invoices][OPEN]);
    menuTrans[invoices][OPEN]->addAnimation(new QPropertyAnimation(dropMenus[invoices][newInvoice], "pos"));
    menuTrans[invoices][OPEN]->addAnimation(new QPropertyAnimation(dropMenus[invoices][openAnInvoice], "pos"));
    menuTrans[invoices][OPEN]->addAnimation(new QPropertyAnimation(dropMenus[invoices][convertToReceipt], "pos"));
    menuTrans[invoices][OPEN]->addAnimation(new QPropertyAnimation(dropMenus[invoices][overdueInvoices], "pos"));

    menuTrans[invoices][CLOSED] = menuStates[invoices][OPEN]->addTransition(this, SIGNAL(homePos()), menuStates[invoices][CLOSED]);
    menuTrans[invoices][CLOSED]->addAnimation(new QPropertyAnimation(dropMenus[invoices][newInvoice], "pos"));
    menuTrans[invoices][CLOSED]->addAnimation(new QPropertyAnimation(dropMenus[invoices][openAnInvoice], "pos"));
    menuTrans[invoices][CLOSED]->addAnimation(new QPropertyAnimation(dropMenus[invoices][convertToReceipt], "pos"));
    menuTrans[invoices][CLOSED]->addAnimation(new QPropertyAnimation(dropMenus[invoices][overdueInvoices], "pos"));

    connect(dropMenus[invoices][newInvoice], SIGNAL(clicked()), this, SLOT(createNewInvoice()));
    connect(dropMenus[invoices][openAnInvoice], SIGNAL(clicked()), this, SLOT(openExistingInvoice()));
    connect(dropMenus[invoices][convertToReceipt], SIGNAL(clicked()), this, SLOT(invoiceToReceipt()));
    connect(dropMenus[invoices][overdueInvoices], SIGNAL(clicked()), this, SLOT(invoicesOverdue()));

    connect(menuTrans[invoices][CLOSED]->animations().back(), SIGNAL(finished()), this, SLOT(lowerMenu()));
}

void MainWindow::initReceiptMenu()
{
    dropMenus[receipts][openAReceipt] = new ChainPanel(":/icons/chaintitle.png", "Find");

    scene->addItem(dropMenus[receipts][openAReceipt]);

    dropMenus[receipts][openAReceipt]->setZValue(21);

    menuStates[receipts] = new QState*[2];

    menuStates[receipts][CLOSED] = new QState(receiptMenuMachine);
    menuStates[receipts][OPEN] = new QState(receiptMenuMachine);

    receiptMenuMachine->setInitialState(menuStates[receipts][CLOSED]);

    // State Receipts OPEN
    menuStates[receipts][OPEN]->assignProperty(dropMenus[receipts][openAReceipt], "pos", QPointF(PLATE_X_OFFSET, -25));

    // State Receipts CLOSED
    menuStates[receipts][CLOSED]->assignProperty(dropMenus[receipts][openAReceipt], "pos", QPointF(PLATE_X_OFFSET, -130));

    menuTrans[receipts] = new QAbstractTransition*[2];

    menuTrans[receipts][OPEN] = menuStates[receipts][CLOSED]->addTransition(this, SIGNAL(openReceipt()), menuStates[receipts][OPEN]);
    menuTrans[receipts][OPEN]->addAnimation(new QPropertyAnimation(dropMenus[receipts][openAReceipt], "pos"));

    menuTrans[receipts][CLOSED] = menuStates[receipts][OPEN]->addTransition(this, SIGNAL(homePos()), menuStates[receipts][CLOSED]);
    menuTrans[receipts][CLOSED]->addAnimation(new QPropertyAnimation(dropMenus[receipts][openAReceipt], "pos"));

    connect(dropMenus[receipts][openAReceipt], SIGNAL(clicked()), this, SLOT(openExistingReceipt()));

    connect(menuTrans[receipts][CLOSED]->animations().back(), SIGNAL(finished()), this, SLOT(lowerMenu()));
}

void MainWindow::initExpenseMenu()
{
    dropMenus[expenses][newExpense] = new ChainPanel(":/icons/chaintitle.png", "New");
    dropMenus[expenses][openAnExpense] = new ChainPanel(":/icons/chaintitle.png", "Find");

    scene->addItem(dropMenus[expenses][openAnExpense]);
    scene->addItem(dropMenus[expenses][newExpense]);

    dropMenus[expenses][newExpense]->setZValue(22);
    dropMenus[expenses][openAnExpense]->setZValue(21);

    menuStates[expenses] = new QState*[2];

    menuStates[expenses][CLOSED] = new QState(expenseMenuMachine);
    menuStates[expenses][OPEN] = new QState(expenseMenuMachine);

    expenseMenuMachine->setInitialState(menuStates[expenses][CLOSED]);

    // State Expenses OPEN
    menuStates[expenses][OPEN]->assignProperty(dropMenus[expenses][newExpense], "pos", QPointF(PLATE_X_OFFSET, -25));
    menuStates[expenses][OPEN]->assignProperty(dropMenus[expenses][openAnExpense], "pos", QPointF(PLATE_X_OFFSET, 25));

    // State Expenses CLOSED
    menuStates[expenses][CLOSED]->assignProperty(dropMenus[expenses][newExpense], "pos", QPointF(PLATE_X_OFFSET, -180));
    menuStates[expenses][CLOSED]->assignProperty(dropMenus[expenses][openAnExpense], "pos", QPointF(PLATE_X_OFFSET, -130));

    menuTrans[expenses] = new QAbstractTransition*[2];

    menuTrans[expenses][OPEN] = menuStates[expenses][CLOSED]->addTransition(this, SIGNAL(openExpense()), menuStates[expenses][OPEN]);
    menuTrans[expenses][OPEN]->addAnimation(new QPropertyAnimation(dropMenus[expenses][newExpense], "pos"));
    menuTrans[expenses][OPEN]->addAnimation(new QPropertyAnimation(dropMenus[expenses][openAnExpense], "pos"));


    menuTrans[expenses][CLOSED] = menuStates[expenses][OPEN]->addTransition(this, SIGNAL(homePos()), menuStates[expenses][CLOSED]);
    menuTrans[expenses][CLOSED]->addAnimation(new QPropertyAnimation(dropMenus[expenses][newExpense], "pos"));
    menuTrans[expenses][CLOSED]->addAnimation(new QPropertyAnimation(dropMenus[expenses][openAnExpense], "pos"));

    connect(dropMenus[expenses][newExpense], SIGNAL(clicked()), this, SLOT(createNewExpense()));
    connect(dropMenus[expenses][openAnExpense], SIGNAL(clicked()), this, SLOT(openExistingExpense()));

    connect(menuTrans[expenses][CLOSED]->animations().back(), SIGNAL(finished()), this, SLOT(lowerMenu()));
}

void MainWindow::initReportMenu()
{
    dropMenus[reports][BAS] = new ChainPanel(":/icons/chaintitle.png", "BAS");
    dropMenus[reports][BASBreakdown] = new ChainPanel(":/icons/chaintitle.png", "Breakdown");
    dropMenus[reports][PandL] = new ChainPanel(":/icons/chaintitle.png", "P & L");

    scene->addItem(dropMenus[reports][PandL]);
    scene->addItem(dropMenus[reports][BASBreakdown]);
    scene->addItem(dropMenus[reports][BAS]);

    dropMenus[reports][BAS]->setZValue(23);
    dropMenus[reports][BASBreakdown]->setZValue(22);
    dropMenus[reports][PandL]->setZValue(21);

    menuStates[reports] = new QState*[2];

    menuStates[reports][CLOSED] = new QState(reportMenuMachine);
    menuStates[reports][OPEN] = new QState(reportMenuMachine);

    reportMenuMachine->setInitialState(menuStates[reports][CLOSED]);

    // State Reports OPEN
    menuStates[reports][OPEN]->assignProperty(dropMenus[reports][BAS], "pos", QPointF(PLATE_X_OFFSET, -25));
    menuStates[reports][OPEN]->assignProperty(dropMenus[reports][BASBreakdown], "pos", QPointF(PLATE_X_OFFSET, 25));
    menuStates[reports][OPEN]->assignProperty(dropMenus[reports][PandL], "pos", QPointF(PLATE_X_OFFSET, 75));

    // State Reports CLOSED
    menuStates[reports][CLOSED]->assignProperty(dropMenus[reports][BAS], "pos", QPointF(PLATE_X_OFFSET, -180));
    menuStates[reports][CLOSED]->assignProperty(dropMenus[reports][BASBreakdown], "pos", QPointF(PLATE_X_OFFSET, -130));
    menuStates[reports][CLOSED]->assignProperty(dropMenus[reports][PandL], "pos", QPointF(PLATE_X_OFFSET, -80));

    menuTrans[reports] = new QAbstractTransition*[2];

    menuTrans[reports][OPEN] = menuStates[reports][CLOSED]->addTransition(this, SIGNAL(openReport()), menuStates[reports][OPEN]);
    menuTrans[reports][OPEN]->addAnimation(new QPropertyAnimation(dropMenus[reports][BAS], "pos"));
    menuTrans[reports][OPEN]->addAnimation(new QPropertyAnimation(dropMenus[reports][BASBreakdown], "pos"));
    menuTrans[reports][OPEN]->addAnimation(new QPropertyAnimation(dropMenus[reports][PandL], "pos"));

    menuTrans[reports][CLOSED] = menuStates[reports][OPEN]->addTransition(this, SIGNAL(homePos()), menuStates[reports][CLOSED]);
    menuTrans[reports][CLOSED]->addAnimation(new QPropertyAnimation(dropMenus[reports][BAS], "pos"));
    menuTrans[reports][CLOSED]->addAnimation(new QPropertyAnimation(dropMenus[reports][BASBreakdown], "pos"));
    menuTrans[reports][CLOSED]->addAnimation(new QPropertyAnimation(dropMenus[reports][PandL], "pos"));

    connect(dropMenus[reports][BAS], SIGNAL(clicked()), this, SLOT(createBAS()));
    connect(dropMenus[reports][BASBreakdown], SIGNAL(clicked()), this, SLOT(createBreakdown()));
    connect(dropMenus[reports][PandL], SIGNAL(clicked()), this, SLOT(createPnL()));

    connect(menuTrans[reports][CLOSED]->animations().back(), SIGNAL(finished()), this, SLOT(lowerMenu()));
}

void MainWindow::lowerMenu()
{
    switch(currentMenuState)
    {
    case quotes:
        emit openQuoteMenu();
        break;
    case invoices:
        emit openInvoice();
        break;
    case receipts:
        emit openReceipt();
        break;
    case expenses:
        emit openExpense();
        break;
    case reports:
        emit openReport();
        break;
    default:
        emit openDefaultMenu();
        break;
    }
}

void MainWindow::updateMenu()
{
    QWidget *widget = ui->stackedWidget->currentWidget();
    HomeWidget *homeWidget = qobject_cast<HomeWidget *>(widget);
    QuoteWidget *quoteWidget = qobject_cast<QuoteWidget *>(widget);
    InvoiceWidget *invoiceWidget = qobject_cast<InvoiceWidget *>(widget);
    ExpensesWidget *expensesWidget = qobject_cast<ExpensesWidget *>(widget);
    ReceiptWidget *receiptWidget = qobject_cast<ReceiptWidget *>(widget);
    ReportingWidget *reportingWidget = qobject_cast<ReportingWidget *>(widget);

    if(homeWidget)
        currentMenuState = homePosition;
    else if(quoteWidget)
        currentMenuState = quotes;
    else if(invoiceWidget)
        currentMenuState = invoices;
    else if(expensesWidget)
        currentMenuState = expenses;
    else if(receiptWidget)
        currentMenuState = receipts;
    else if(reportingWidget)
        currentMenuState = reports;

    emit homePos();
    lowerMenu();
}

void MainWindow::setQuoteMenu()
{
    ui->listWidget->setCurrentRow(0);
    if(currentMenuState != quotes)
    {
        currentMenuState = quotes;
        emit homePos();
    }

}

void MainWindow::setInvoiceMenu()
{
    ui->listWidget->setCurrentRow(0);
    if(currentMenuState != invoices)
    {
        currentMenuState = invoices;
        emit homePos();
    }
}

void MainWindow::setReceiptMenu()
{
    ui->listWidget->setCurrentRow(0);
    if(currentMenuState != receipts)
    {
        currentMenuState = receipts;
        emit homePos();
    }
}

void MainWindow::setExpenseMenu()
{
    ui->listWidget->setCurrentRow(0);
    if(currentMenuState != expenses)
    {
        currentMenuState = expenses;
        emit homePos();
    }
}

void MainWindow::setReportMenu()
{
    ui->listWidget->setCurrentRow(0);
    if(currentMenuState != reports)
    {
        currentMenuState = reports;
        emit homePos();
    }
}

void MainWindow::setHomePos()
{
    emit homePos();
}

void MainWindow::setInvoicePos()
{
    emit openInvoice();
}

void MainWindow::setReceiptPos()
{
    emit openReceipt();
}

void MainWindow::setExpensePos()
{
    emit openExpense();
}

void MainWindow::setReportPos()
{
    emit openReport();
}

void MainWindow::closeDrawers()
{
    emit drawerHome();
}

void MainWindow::quotePaperHome()
{
    emit quotePaperHomePos();
}
