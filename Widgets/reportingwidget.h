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
