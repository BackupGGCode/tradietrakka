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
