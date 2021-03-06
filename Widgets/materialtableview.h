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

#ifndef MATERIALTABLEVIEW_H
#define MATERIALTABLEVIEW_H

#include <QTableView>
//#include <QKeyEvent>

class MaterialTableView : public QTableView
{
    Q_OBJECT

    int ident;

public:
    explicit MaterialTableView(QWidget *parent = 0);

    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

signals:
    void keyPress(QKeyEvent *);
    void rightClick();

};

#endif // MATERIALTABLEVIEW_H
