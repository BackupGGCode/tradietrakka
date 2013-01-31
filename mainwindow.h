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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define QUOTES_MENU_NUM 4
#define INVOICES_MENU_NUM 4
#define RECEIPTS_MENU_NUM 1
#define EXPENSES_MENU_NUM 2
#define REPORTS_MENU_NUM 3

#define OPEN 0
#define CLOSED 1

#include <QMainWindow>
#include <QSplashScreen>

#include "appsettings.h"
#include "dbasectrl.h"
#include "homewidget.h"
#include "quotewidget.h"
#include "invoicewidget.h"
#include "receiptwidget.h"
#include "reportingwidget.h"
#include "expenseswidget.h"
#include "addcontactdialog.h"
#include "companydialog.h"
#include "finddialog.h"
#include "contactdialog.h"
#include "settingsdialog.h"

enum PaperDrawer {quotes, invoices, receipts, expenses, reports,
                  invoiceHome, receiptHome, expenseHome, reportHome, homePosition};

enum QuoteMenu {newQuote, openQuote, convertToInvoice, deleteQuote};
enum InvoiceMenu {newInvoice, openAnInvoice, convertToReceipt, overdueInvoices};
enum ReceiptMenu {openAReceipt};
enum ExpenseMenu {newExpense, openAnExpense};
enum ReportMenu {BAS, BASBreakdown, PandL};

class ImgObject;

class QState;
class QProcess;
class QTimeLine;
class QStateMachine;
class QGraphicsScene;
class QSignalTransition;
class QAbstractTransition;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

    QSplashScreen *splash;
    Qt::Alignment bottomLeft;

    PaperDrawer currentMenuState;
    bool opening;
    ReportingWidget::reportType reportType;
    QString reportLabel;

    AppSettings *appSettings;
    DBaseCtrl *dbaseCtrl;

    QGraphicsScene *scene;

    QStateMachine *machine;
    QStateMachine *paperMachine;
    QStateMachine *defaultMenuMachine;
    QStateMachine *quoteMenuMachine;
    QStateMachine *invoiceMenuMachine;
    QStateMachine *receiptMenuMachine;
    QStateMachine *expenseMenuMachine;
    QStateMachine *reportMenuMachine;

    QState **paperStates;
    QState **defaultMenuStates;
    QState **drawStates;
    QState ***menuStates;

    QAbstractTransition **paperTrans;
    QAbstractTransition **defaultMenuTrans;
    QAbstractTransition **drawerTrans;
    QAbstractTransition ***menuTrans;

    QProcess *calcProcess;

    HomeWidget *home;

public:
    ImgObject *defaultMenuItem;
    ImgObject *paperItem;
    ImgObject **paperDrawers;
    ImgObject **drawerFronts;
    ImgObject ***dropMenus;

    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void initChecks();
    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *e);
    void initMenu();
    void initObjects();
    void initPaper();
    void initDrawers();
    void initDropMenus();
    void initDefaultMenu();
    void initQuoteMenu();
    void initInvoiceMenu();
    void initReceiptMenu();
    void initExpenseMenu();
    void initReportMenu();
    void initStack();

    void removeCurrentItem(int index);

private:
    Ui::MainWindow *ui;

signals:
    void homePos();
    void quotePaperHomePos();
    void drawerHome();
    void openDefaultMenu();
    void openQuoteMenu();
    void openInvoice();
    void quotePaperPos();
    void invoicePaperPos();
    void receiptPaperPos();
    void expensePaperPos();
    void reportPaperPos();
    void openReceipt();
    void openExpense();
    void openReport();

    void openInvoiceDraw();
    void openReceiptDraw();
    void openExpenseDraw();
    void openReportDraw();

    void paperToQuote();
    void paperFromQuote();
    void paperToInvoice();
    void paperFromInvoice();
    void paperToReceipt();
    void paperFromReceipt();
    void paperToExpense();
    void paperFromExpense();
    void paperToReport();
    void paperFromReport();

public slots:
    void lowerMenu();
    void updateMenu();

    void setQuoteMenu();
    void setInvoiceMenu();
    void setReceiptMenu();
    void setExpenseMenu();
    void setReportMenu();

    void setHomePos();
    void setInvoicePos();
    void setReceiptPos();
    void setExpensePos();
    void setReportPos();
    void closeDrawers();
    void quotePaperHome();

    void aboutQtDialog();
    void aboutDialog();

    void createNewQuote();
    void openNewQuote();
    void openExistingQuote();
    void quoteToInvoice();
    void quoteDelete();

    void createNewInvoice();
    void invoiceDrawerClosed();
    void openNewInvoice();
    void openExistingInvoice();
    void invoiceToReceipt();
    void invoicesOverdue();

    void createNewReceipt();
    void receiptDrawerClosed();
    void openNewReceipt();
    void openExistingReceipt();

    void createNewExpense();
    void expenseDrawerClosed();
    void openNewExpense();
    void openExistingExpense();


    void createBAS();
    void createPnL();
    void createBreakdown();

    void createNewReport();
    void reportDrawerClosed();
    void openNewReport();

    void launchCalc();

    void addPersonalContact();
    void addCompanyContact();

private slots:
    void on_printButton_clicked();
    void on_pdfButton_clicked();
    void on_emailButton_clicked();
    void on_closeButton_clicked();
    void findContacts(ContactDialog::contactType);
    void findContacts();
    void editSettings();
    void materialsEdit();
    void labourEdit();
    void updateOverDueInvoice();
};

#endif // MAINWINDOW_H
