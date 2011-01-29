#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include "contactdialog.h"
#include "labourchargedialog.h"
#include "materialpricesdialog.h"

#include <QWidget>
#include <QSqlDatabase>

namespace Ui {
    class HomeWidget;
}

class HomeWidget : public QWidget
{
    Q_OBJECT

    QSqlDatabase dbase;
    QPixmap pixmap;

public:
    explicit HomeWidget(QSqlDatabase dbase, QWidget *parent = 0);
    ~HomeWidget();

    void resizeEvent(QResizeEvent *);

private:
    Ui::HomeWidget *ui;

signals:
    void businessButtonClicked(ContactDialog::contactType);
    void personalButtonClicked(ContactDialog::contactType);
    void labourButtonClicked();
    void materialsButtonClicked();

private slots:
    void on_contactsButton_clicked();
    void on_labourButton_clicked();
    void on_materialButton_clicked();
    void on_businessButton_clicked();
};

#endif // HOMEWIDGET_H
