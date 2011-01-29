#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QDataWidgetMapper>

#include "dbasectrl.h"
#include "appsettings.h"

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

    QImage logoImage;
    DBaseCtrl *dbCtrl;
    QSqlRelationalTableModel *ownerModel;
//    QSqlTableModel *stateRelation;

    QDataWidgetMapper *mapper;
    AppSettings appSettings;

public:
    explicit SettingsDialog(DBaseCtrl *dbCtrl, QWidget *parent = 0);
    ~SettingsDialog();

private:
    Ui::SettingsDialog *ui;
    void loadSettings();
    void updateLogo();
    void commit();

private slots:
    void on_changeDBButton_clicked();
    void on_buttonBox_clicked(QAbstractButton* button);
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_clearButton_clicked();
    void on_browseButton_clicked();
};

#endif // SETTINGSDIALOG_H
