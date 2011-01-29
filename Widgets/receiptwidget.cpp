#include "receiptwidget.h"
#include "ui_receiptwidget.h"
#include "dbasectrl.h"

#include <QSqlRecord>
#include <QSqlTableModel>
#include <QDebug>

ReceiptWidget::ReceiptWidget(int receiptID, DBaseCtrl *dbaseCtrl, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReceiptWidget)
{
    ui->setupUi(this);
    this->receiptID = receiptID;

    QSqlTableModel *receiptModel = dbaseCtrl->getReceiptModel();

    QSqlRecord record;
    for(int i = 0; i < receiptModel->rowCount(); i++)
    {
        record = receiptModel->record(i);
        if(record.value(DBaseCtrl::Receipt_ID).toInt() == receiptID)
            break;
    }

    QImage logo = dbaseCtrl->getLogo();
    ui->receiptBrowser->document()->addResource(QTextDocument::ImageResource, QUrl("logo://logo"), QVariant(logo));
    ui->receiptBrowser->setHtml(record.value(DBaseCtrl::Receipt_Details).toString());
}

ReceiptWidget::~ReceiptWidget()
{
    delete ui;
}
