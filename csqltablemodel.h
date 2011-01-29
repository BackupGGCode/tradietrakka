#ifndef CSQLTABLEMODEL_H
#define CSQLTABLEMODEL_H

#include <QSqlTableModel>
#include <QSqlRecord>
#include <QDebug>

class CSqlTableModel : public QSqlTableModel
{
    Q_OBJECT

    int totalCol, firstCol, secondCol;

public:
    CSqlTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

    QVariant data(const QModelIndex &item, int role) const;

    void insertTotalCol();
    void setCols(int firstCol, int secondCol, int totalCol);
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
};

#endif // CSQLTABLEMODEL_H
