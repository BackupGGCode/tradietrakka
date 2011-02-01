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
