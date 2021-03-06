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

#include "reportingwidget.h"
#include "ui_reportingwidget.h"

#include "printcomposer.h"

#define FISCAL_DAY 1
#define FISCAL_MON 7

ReportingWidget::ReportingWidget(reportType report, DBaseCtrl *dbCtrl, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReportingWidget)
{
    ui->setupUi(this);

    this->dbCtrl = dbCtrl;

    QImage logo = dbCtrl->getLogo();
    ui->reportBrowser->document()->addResource(QTextDocument::ImageResource, QUrl("logo://logo"), QVariant(logo));
    objectType = report;

    switch (report)
    {
    case Bas:
        initBas();
        break;
    case PnL:
        initPnL();
        break;
    case Breakdown:
        initBreakdown();
        break;

    }
}

ReportingWidget::~ReportingWidget()
{
    delete ui;
}

void ReportingWidget::initBas()
{
    ui->reportLabel->setText("BAS Statement Report");
    ui->reportPeriod->setTitle("BAS Period");

    //setup default date periods, current to 3 months prior
    ui->endReportDate->setDate(QDate::currentDate());
    ui->startReportDate->setDate(QDate::currentDate().addMonths(-3).addDays(-(QDate::currentDate().day() - 1)));

    //set max dates
    ui->endReportDate->setMaximumDate(QDate::currentDate());
    ui->startReportDate->setMaximumDate(QDate::currentDate());

}

void ReportingWidget::initPnL()
{
    ui->reportLabel->setText("Profit and Loss Report");
    ui->reportPeriod->setTitle("Profit && Loss Period");

    //fiscal year start
    ui->endReportDate->setDate(QDate::currentDate());

    //if the date is past the fiscal monthkeep current year
    int fiscalYear = QDate::currentDate().year();
    if(QDate::currentDate().month() < 6)
        fiscalYear -= 1;

    QDate fiscalDate(fiscalYear, FISCAL_MON, FISCAL_DAY);
    ui->startReportDate->setDate(fiscalDate);
}

void ReportingWidget::initBreakdown()
{    
    ui->reportLabel->setText("Breakdown Report");
    ui->reportPeriod->setTitle("Breakdown Period");

    //setup default date periods, current to 3 months prior
    ui->endReportDate->setDate(QDate::currentDate());
    ui->startReportDate->setDate(QDate::currentDate().addMonths(-3).addDays(-(QDate::currentDate().day() - 1)));

    //set max dates
    ui->endReportDate->setMaximumDate(QDate::currentDate());
    ui->startReportDate->setMaximumDate(QDate::currentDate());
}

void ReportingWidget::on_generateReportButton_clicked()
{
    QString reportHtml;
    switch(objectType)
    {
    case Bas:
        reportHtml = PrintComposer::basString(ui->startReportDate->date(), ui->endReportDate->date(), dbCtrl);
        break;
    case PnL:
        reportHtml = PrintComposer::pandlString(ui->startReportDate->date(), ui->endReportDate->date(), dbCtrl);
        break;
    case Breakdown:
        reportHtml = PrintComposer::breakdownString(ui->startReportDate->date(), ui->endReportDate->date(), dbCtrl);
        break;
    }

    ui->reportBrowser->setHtml(reportHtml);
}

QString ReportingWidget::getHtml()
{
    return ui->reportBrowser->toHtml();
}
