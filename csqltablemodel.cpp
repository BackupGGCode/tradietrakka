/*******************************************************************************

  TradieTrakka - Easy to use small business management software
  Copyright (C) 2010 Craig Hume

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

#include "csqltablemodel.h"

CSqlTableModel::CSqlTableModel(QObject *parent, QSqlDatabase db) :
        QSqlTableModel(parent, db)
{
    firstCol = secondCol = totalCol = -1;
}

QVariant CSqlTableModel::data(const QModelIndex &item, int role) const
{
    if (item.column() == totalCol && role==Qt::DisplayRole && totalCol != -1)
    {
        if(record(item.row()).value(firstCol).isNull() ||
           record(item.row()).value(secondCol).isNull())
           return QSqlTableModel::data(item, role);

        int qty = record(item.row()).value(firstCol).toInt();
        double price = record(item.row()).value(secondCol).toDouble();
        int transTotal = qty * price * 100;
        double total = transTotal / 100.00;
        return QVariant(total);
    }
    return QSqlTableModel::data(item, role);
}

void CSqlTableModel::insertTotalCol()
{
    QSqlTableModel::insertColumn(totalCol);
    QSqlTableModel::setHeaderData(totalCol, Qt::Horizontal, "Sub Total");
}

void CSqlTableModel::setCols(int firstCol, int secondCol, int totalCol)
{
    this->firstCol = firstCol;
    this->secondCol = secondCol;
    this->totalCol = totalCol;
}

bool CSqlTableModel::insertRow(int row, const QModelIndex &parent)
{
    bool success = QSqlTableModel::insertRow(row, parent);

    insertTotalCol();

    return success;
}
