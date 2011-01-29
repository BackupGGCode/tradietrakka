#ifndef RECEIPTWIDGET_H
#define RECEIPTWIDGET_H

#include <QWidget>

class DBaseCtrl;

namespace Ui {
    class ReceiptWidget;
}

class ReceiptWidget : public QWidget
{
    Q_OBJECT

    int receiptID;
    DBaseCtrl *dbaseCtrl;
public:
    explicit ReceiptWidget(int receiptID, DBaseCtrl *dbaseCtrl, QWidget *parent = 0);
    ~ReceiptWidget();
    int getReceiptID(){ return receiptID; }


private:
    Ui::ReceiptWidget *ui;
};

#endif // RECEIPTWIDGET_H
