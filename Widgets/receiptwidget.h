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
