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

#ifndef REPORTINGWIDGET_H
#define REPORTINGWIDGET_H

#include <QWidget>

#include "dbasectrl.h"

namespace Ui {
    class ReportingWidget;
}

class ReportingWidget : public QWidget
{
    Q_OBJECT

    DBaseCtrl *dbCtrl;
public:
    enum reportType{Bas, PnL, Breakdown};

    explicit ReportingWidget(reportType report, DBaseCtrl *dbCtrl, QWidget *parent = 0);
    ~ReportingWidget();

    QString getHtml();

private:
    Ui::ReportingWidget *ui;

    void initBas();
    void initPnL();
    void initBreakdown();


    reportType objectType;


private slots:
    void on_generateReportButton_clicked();
};

#endif // REPORTINGWIDGET_H
