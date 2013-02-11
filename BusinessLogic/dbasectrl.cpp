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

#include<QDebug>
#include <QDir>
#include <QDate>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QBuffer>
#include <QImageWriter>
#include <QImageReader>
#include <QVariant>

#include "dbasectrl.h"

/**
  * Constructor
  */
DBaseCtrl::DBaseCtrl(AppSettings *appSettings, QObject *parent)
    : QObject(parent)
{
    this->appSettings = appSettings;
    dbaseName = "Primary";

}

/**
  * Destructor
  */
DBaseCtrl::~DBaseCtrl()
{

}



/**
  * Initilize all table models
  */
void DBaseCtrl::initDBModels()
{
    initPersonModel();
    initCompanyModel();
    initAddressModel();
    initStateModel();
    initMaterialsModel();
    initLabourModel();
    initQuoteModel();
    initInvoiceModel();
    initReceiptModel();
    initExpenseModel();
    initOwnerModel();

}

void DBaseCtrl::insertPersonAtCompany(int companyId, int personId)
{
    QSqlQuery query(dbase);
    query.prepare("INSERT INTO PersonAtCompany "
                  "VALUES (NULL, " + QString::number(companyId) + ", " + QString::number(personId) + ")");
    if(!query.exec())
    {
        qWarning("Insert into PersonAtCompany did not succeed!?!?");
    }

}

void DBaseCtrl::removePersonAtCompany(int id)
{
    QSqlQuery query(dbase);
    QString queryString = "DELETE FROM PersonAtCompany "
                          "WHERE person = " + QString::number(id);

    query.prepare(queryString);

    if(!query.exec())
    {
        qWarning("Delete from PersonAtCompany did not succeed!?!?");
    }
}

void DBaseCtrl::insertAddressOfCompany(int companyID, int addressID)
{
    QSqlQuery query(dbase);
    query.prepare("INSERT INTO AddressOfCompany "
                  "VALUES (NULL, " + QString::number(addressID) +
                  ", " + QString::number(companyID) + ")");
    if(!query.exec())
    {
        qWarning("Insert into AddressOfCompany did not succeed!?!?");
    }
}

void DBaseCtrl::removeAddressOfCompany(int addressID)
{
    QSqlQuery query(dbase);
    QString queryString = "DELETE FROM AddressOfCompany "
                          "WHERE address = " + QString::number(addressID);

    query.prepare(queryString);

    if(!query.exec())
    {
        qWarning("Delete from AddressOfCompany did not succeed!?!?");
    }

}


void DBaseCtrl::insertAddressOfPerson(int personID, int addressID)
{
    QSqlQuery query(dbase);

    query.prepare("INSERT INTO AddressOfPerson "
                  "VALUES (NULL, " + QString::number(addressID) +
                  ", " + QString::number(personID) + ")");
    if(!query.exec())
    {
        qWarning("Insert into AddressOfPErson did not succeed!?!?");
    }

}

void DBaseCtrl::removeAddressOfPerson(int addressID, int personID)
{
    QSqlQuery query(dbase);

    QString queryString = "DELETE FROM AddressOfPerson "
                          "WHERE address = " + QString::number(addressID) + " " +
                          "AND person = " + QString::number(personID);

    query.prepare(queryString);

    if(!query.exec())
    {
        qWarning("Delete from AddressOfPerson did not succeed!?!?");
    }
}

void DBaseCtrl::removeAllAddressesOfPerson(int personID)
{
    QSqlQuery query(dbase);

    QString queryString = "DELETE FROM AddressOfPerson "
                          "WHERE person = " + QString::number(personID);

    query.prepare(queryString);


    if(!query.exec())
    {
        qWarning("Delete from all AddressOfPerson did not succeed!?!?");
    }
}

bool DBaseCtrl::removeQuote(int quoteID)
{
    QSqlTableModel *deleteQuoteModel = new QSqlTableModel(this, dbase);

    deleteQuoteModel->setTable("Quote");
    deleteQuoteModel->setFilter("id = " + QString::number(quoteID));
    deleteQuoteModel->select();

    CSqlTableModel *loQ = this->getLabourOnQuoteModel(quoteID);
    CSqlTableModel *moQ = this->getMaterialsOnQuoteModel(quoteID);

    bool success = true;

    dbase.transaction();
    moQ->removeRows(0, moQ->rowCount());

    loQ->removeRows(0, loQ->rowCount());

    deleteQuoteModel->removeRow(0);

    success = moQ->submitAll();

    if(success)
        success = loQ->submitAll();

    if(success)
        success = deleteQuoteModel->submitAll();

    if(success)
        dbase.commit();
    else
        dbase.rollback();

    return success;
}

/**
  * Initilize the person model
  */
void DBaseCtrl::initPersonModel()
{
    personModel = new QSqlTableModel(this, dbase);
    personModel->setTable("Person");
    personModel->setHeaderData(1, Qt::Horizontal, "First Name");
    personModel->setHeaderData(2, Qt::Horizontal, "Last Name");
    personModel->select();
}

/**
  *
  */
void DBaseCtrl::initCompanyModel()
{
    companyModel = new QSqlTableModel(this, dbase);
    companyModel->setTable("Company");
    companyModel->setHeaderData(1, Qt::Horizontal, "Company Name");
    companyModel->select();
}

void DBaseCtrl::initAddressModel()
{
    addressModel = new QSqlRelationalTableModel(this, dbase);

    addressModel->setTable("address");
    addressModel->setRelation(8, QSqlRelation("state", "id", "name"));
    addressModel->select();
}

void DBaseCtrl::initStateModel()
{
    stateModel = new QSqlTableModel(this, dbase);
    stateModel->setTable("state");
    stateModel->select();
}

/**
  *
  */
void DBaseCtrl::initMaterialsModel()
{
    materialsModel = new QSqlTableModel(this, dbase);
    materialsModel->setTable("MaterialPrices");
    materialsModel->select();
}

/**
  *
  */
void DBaseCtrl::initLabourModel()
{
    labourModel = new QSqlTableModel(this, dbase);
    labourModel->setTable("LabourRates");
    labourModel->select();
}

void DBaseCtrl::initQuoteModel()
{
    quoteModel = new QSqlTableModel(this, dbase);
    quoteModel->setTable("quote");
    quoteModel->select();
}

void DBaseCtrl::initInvoiceModel()
{
    invoiceModel = new QSqlTableModel(this, dbase);
    invoiceModel->setTable("Invoice");
    invoiceModel->select();
}

void DBaseCtrl::initReceiptModel()
{
    receiptModel = new QSqlTableModel(this, dbase);
    receiptModel->setTable("Receipt");
    receiptModel->select();
}

void DBaseCtrl::initExpenseModel()
{
    expenseModel = new QSqlTableModel(this, dbase);
    expenseModel->setTable("Expense");
    expenseModel->select();
}

void DBaseCtrl::initQuoteBridgeModels()
{

}

CSqlTableModel* DBaseCtrl::getMaterialsOnQuoteModel(int quoteID)
{
    CSqlTableModel *materialsOnQuoteModel = new CSqlTableModel(this, dbase);
    materialsOnQuoteModel->setTable("MaterialsOnQuote");
    materialsOnQuoteModel->setFilter("quoteID = " + QString::number(quoteID));
    materialsOnQuoteModel->select();

    return materialsOnQuoteModel;
}

CSqlTableModel* DBaseCtrl::getLabourOnQuoteModel(int quoteID)
{
    CSqlTableModel *labourOnQuoteModel = new CSqlTableModel(this, dbase);
    labourOnQuoteModel->setTable("LabourOnQuote");
    labourOnQuoteModel->setFilter("quoteID = " + QString::number(quoteID));
    labourOnQuoteModel->select();

    return labourOnQuoteModel;
}

CSqlTableModel* DBaseCtrl::getMaterialsOnInvoiceModel(int invoiceID)
{
    CSqlTableModel *materialsOnInvoiceModel = new CSqlTableModel(this, dbase);
    materialsOnInvoiceModel->setTable("MaterialsOnInvoice");
    materialsOnInvoiceModel->setFilter("invoiceID = " + QString::number(invoiceID));
    materialsOnInvoiceModel->select();

    return materialsOnInvoiceModel;
}

CSqlTableModel* DBaseCtrl::getLabourOnInvoiceModel(int invoiceID)
{
    CSqlTableModel *labourOnInvoiceModel = new CSqlTableModel(this, dbase);
    labourOnInvoiceModel->setTable("LabourOnInvoice");
    labourOnInvoiceModel->setFilter("invoiceID = " + QString::number(invoiceID));
    labourOnInvoiceModel->select();

    return labourOnInvoiceModel;
}

/**
  *
  */
QSqlTableModel* DBaseCtrl::getPersonModel()
{
    return personModel;
}

/**
  *
  */
QSqlTableModel* DBaseCtrl::getCompanyModel()
{
    return companyModel;
}

/**
  *
  */
QSqlTableModel* DBaseCtrl::getMaterialsModel()
{
    return materialsModel;
}

QSqlTableModel* DBaseCtrl::getLabourModel()
{
    return labourModel;
}

QSqlTableModel* DBaseCtrl::getQuoteModel()
{
    return quoteModel;
}

QSqlTableModel* DBaseCtrl::getInvoiceModel()
{
    return invoiceModel;
}

QSqlTableModel* DBaseCtrl::getReceiptModel()
{
    return receiptModel;
}

QSqlTableModel* DBaseCtrl::getExpenseModel()
{
    return expenseModel;
}

/**
  *
  */
QSqlRelationalTableModel* DBaseCtrl::getAddressModel()
{
    return addressModel;
}

/**
  *
  */
QSqlTableModel* DBaseCtrl::getSuburbModel()
{
    return suburbModel;
}

/**
  *
  */
QSqlTableModel* DBaseCtrl::getStateModel()
{
    return stateModel;
}

QSqlQueryModel* DBaseCtrl::getSingleAddressModel(int addressID)
{
    QSqlQueryModel *addressQueryModel = new QSqlQueryModel(this);

    QString query = "SELECT * FROM address "
                    "WHERE id = " + QString::number(addressID);

    addressQueryModel->setQuery(query, dbase);

    return addressQueryModel;
}

QSqlQueryModel* DBaseCtrl::getSingleQuoteModel(int quoteID)
{
    QSqlQueryModel *singleQuoteModel = new QSqlQueryModel(this);
    QString query = "SELECT * FROM quote "
                    "WHERE id = " + QString::number(quoteID);

    singleQuoteModel->setQuery(query, dbase);

    return singleQuoteModel;
}

QSqlQueryModel* DBaseCtrl::getSingleInvoiceModel(int invoiceID)
{
    QSqlQueryModel *singleInvoiceModel = new QSqlQueryModel(this);
    QString query = "SELECT * FROM invoice "
                    "WHERE id = " + QString::number(invoiceID);

    singleInvoiceModel->setQuery(query, dbase);

    return singleInvoiceModel;
}

QSqlQueryModel* DBaseCtrl::getSingleReceiptModel(int receiptID)
{
    QSqlQueryModel *singleReceiptModel = new QSqlQueryModel(this);
    QString query = "SELECT * FROM Receipt "
                    "WHERE id = " + QString::number(receiptID);

    singleReceiptModel->setQuery(query, dbase);

    return singleReceiptModel;
}

QSqlQueryModel* DBaseCtrl::getSingleCompanyModel(int companyID)
{
    QSqlQueryModel *companyQueryModel = new QSqlQueryModel(this);

    QString query = "SELECT * FROM company "
                    "WHERE id = " + QString::number(companyID);

    companyQueryModel->setQuery(query, dbase);

    return companyQueryModel;

}
QSqlQueryModel* DBaseCtrl::getSinglePersonModel(int personID)
{
    QSqlQueryModel *contactQueryModel = new QSqlQueryModel(this);

    QString query = "SELECT * FROM person "
                    "WHERE id = " + QString::number(personID);

    contactQueryModel->setQuery(query, dbase);

    return contactQueryModel;
}

QSqlQueryModel* DBaseCtrl::getSingleExpenseModel(int expenseID)
{
    QSqlQueryModel *singleExpenseModel = new QSqlQueryModel(this);

    QString query = "SELECT * FROM expense "
                    "WHERE id = " + QString::number(expenseID);

    singleExpenseModel->setQuery(query, dbase);

    return singleExpenseModel;
}

QStringList DBaseCtrl::getQuoteEmailAndNum(int quoteID)
{
    // Need to select email and id from Quote
    // and place into QStringList

    QStringList emailAndNum;
    emailAndNum << "a_name@a_domain.com" << "123456";

    return emailAndNum;
}

int DBaseCtrl::getNumberOfOverDue()
{
    int overDue;

    QSqlQuery countQuery(this->dbase);

    QString queryString = "SELECT COUNT(*) from Invoice "
                          "WHERE status < 3 and "
                          "dueDate < " + QDate::currentDate().toString("yyyyMMdd");

    countQuery.exec(queryString);

    countQuery.next();

    overDue = countQuery.value(0).toInt();

    return overDue;
}

QSqlQueryModel* DBaseCtrl::getCompanyContactsModel(int companyId)
{
    QSqlQueryModel *companyContactsModel = new QSqlQueryModel(this);
    QString query = "Select Person.id, Person.fName, Person.lName from Person, PersonAtCompany "
                    "where Person.id = PersonAtCompany.person and "
                    "PersonAtCompany.company = " + QString::number(companyId);

    companyContactsModel->setQuery(query, dbase);
    companyContactsModel->setHeaderData(1, Qt::Horizontal, "First Name");
    companyContactsModel->setHeaderData(2, Qt::Horizontal, "Last Name");


    return companyContactsModel;
}

QSqlQueryModel* DBaseCtrl::getCompanyAddressesModel(int companyId)
{
    QSqlQueryModel *companyAddressesModel = new QSqlQueryModel(this);

    QString query = "SELECT address.id, address.street, address.suburb FROM address, addressofcompany "
                    "WHERE address.id = addressofcompany.address "
                    "AND addressofcompany.company = " + QString::number(companyId);

    companyAddressesModel->setQuery(query, dbase);
    companyAddressesModel->setHeaderData(1, Qt::Horizontal, "Street");
    companyAddressesModel->setHeaderData(2, Qt::Horizontal, "Suburb");

    return companyAddressesModel;
}

QSqlQueryModel* DBaseCtrl::getPersonAddressModel(int personID)
{
    QSqlQueryModel *personAddressModel = new QSqlQueryModel(this);

    QString query = "SELECT address.id, address.street, address.suburb "
                    "FROM address, addressofperson "
                    "WHERE address.id = addressofperson.address "
                    "AND addressofperson.person = " + QString::number(personID);

    personAddressModel->setQuery(query, dbase);
    personAddressModel->setHeaderData(1, Qt::Horizontal, "Street");
    personAddressModel->setHeaderData(2, Qt::Horizontal, "Suburb");

    return personAddressModel;
}

QImage DBaseCtrl::getLogo()
{
    QSqlQuery query(dbase);

    QImage image;
    query.prepare("SELECT logoImage FROM owner WHERE id = 1");

    if(!query.exec())
        qWarning("Could not retrive logoImage from owner table");
    else
    {
        if(query.next())
        {
            QByteArray array = query.value(0).toByteArray();
            image.loadFromData(array);
            return image;
        }
    }
    return image;
}

void DBaseCtrl::setLogo(QImage *logo)
{
    QBuffer buffer;
    QImageWriter writer(&buffer, "PNG");
    writer.write(*logo);
    QByteArray image = buffer.data();

    QSqlQuery query(dbase);

    query.prepare("UPDATE Owner SET logoImage = :logo WHERE id = 1");
    query.bindValue(":logo", QVariant(image), QSql::In);

    if(!query.exec())
    {
        qWarning("Could not write logoImage to owner table");
        qDebug() << query.lastError().text();
    }
}

QSqlRelationalTableModel *DBaseCtrl::getOwnerModel()
{
    return ownerModel;
}

void DBaseCtrl::initOwnerModel()
{
    ownerModel = new QSqlRelationalTableModel(this, dbase);
    ownerModel->setTable("Owner");
    ownerModel->setRelation(DBaseCtrl::Owner_State, QSqlRelation("state", "id", "name"));

    if(!ownerModel->select())
        qDebug() << "Error in owner model" << ownerModel->lastError().text();
}

QSqlDatabase DBaseCtrl::getDbase()
{
    return dbase;
}

/**
  *
  */
bool DBaseCtrl::connect(const bool &createDb)
{
    this->removeDatabase();
    dbase = QSqlDatabase::addDatabase("QSQLITE", dbaseName);
    QFileInfo fileInfo(appSettings->getDbPath() + QDir::separator() + appSettings->getDbName());

    if(!fileInfo.exists())
    {
        qWarning() << "Database file not found";
        if(!createDb){
            qWarning("Could not create the database file");
            return false;
        }
        qWarning("Creating Database File ... ");
    }

    dbase.setConnectOptions("QSQLITE_BUSY_TIMEOUT=1");
    dbase.setDatabaseName(fileInfo.absoluteFilePath());

    if(!dbase.open())
    {
        qWarning() << "Could not open Database";
        return false;
    }
    return true;
}

/**
  *
  */
void DBaseCtrl::removeDatabase()
{
    dbase.close();
    dbase = QSqlDatabase();
    QSqlDatabase::removeDatabase(dbaseName);
}

/**
  *
  */
bool DBaseCtrl::isOpen()
{
    return dbase.isOpen();
}

/**
  *
  */
bool DBaseCtrl::hasTables()
{
    return !(dbase.tables().isEmpty());
}

/**
  *
  */
bool DBaseCtrl::createTables()
{
    if(this->hasTables())
    {
        return false;
    }

    QSqlQuery query(dbase);

    query.prepare("CREATE TABLE IF NOT EXISTS State "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, "
                  "name VARCHAR(40) NOT NULL, "
                  "shortName CHAR(8) NOT NULL"
                  ")");

    if(!query.exec())
        qWarning("Could not create State table");


    query.prepare("CREATE TABLE IF NOT EXISTS Address "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, "
                  "unitNum INTEGER, "
                  "houseNum INTEGER, "
                  "POBox VARCHAR(10), "
                  "street VARCHAR(40), "
                  "suburb VARCHAR(40), "
                  "postcode VARCHAR(5), "
                  "city VARCHAR(40), "
                  "state INTEGER, "
                  "FOREIGN KEY (state) REFERENCES State(id)"
                  ")");

    if(!query.exec())
        qWarning("Could not create Address table");


    query.prepare("CREATE TABLE IF NOT EXISTS Company "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, "
                  "name VARCHAR(80) NOT NULL, "
                  "phone VARCHAR(20), "
                  "fax VARCHAR(20), "
                  "email VARCHAR(80), "
                  "website VARCHAR(50), "
                  "abn VARCHAR(20), "
                  "Address INTEGER, "
                  "FOREIGN KEY (address) REFERENCES Address(id)"
                  ")");

    if(!query.exec())
        qWarning("Could not create Company table");


    query.prepare("CREATE TABLE IF NOT EXISTS Person "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, "
                  "fName VARCHAR(80) NOT NULL, "
                  "lName VARCHAR(80) NOT NULL, "
                  "homePh VARCHAR(20), "
                  "officePh VARCHAR(20), "
                  "mobile VARCHAR(20), "
                  "fax VARCHAR(20), "
                  "email VARCHAR(50), "
                  "abn VARCHAR(20) "
                  ")");

    if(!query.exec())
        qWarning("Could not create Person table");


    query.prepare("CREATE TABLE IF NOT EXISTS PersonAtCompany "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, "
                  "company INTEGER NOT NULL, "
                  "person INTEGER NOT NULL, "
                  "FOREIGN KEY (company) REFERENCES Company(id), "
                  "FOREIGN KEY (person) REFERENCES Person(id)"
                  ")");

    if(!query.exec())
        qWarning("Could not create PersonAtCompany table");


    query.prepare("CREATE TABLE IF NOT EXISTS Quote "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, "
                  "qNum INTEGER, "
                  "qDate TEXT NOT NULL, "
                  "expiryDate TEXT, "
                  "details CLOB, "
                  "entityType INTEGER NOT NULL, " //0(company) or 1(person)
                  "entityID INTEGER NOT NULL, "
                  "addressID INTEGER, "
                  "FOREIGN KEY (addressID) REFERENCES Address(id)"
                  ")");

    if(!query.exec())
        qWarning("Could not create Quote table");


    query.prepare("CREATE TABLE IF NOT EXISTS Invoice "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, "
                  "iNum INTEGER, "
                  "iDate TEXT NOT NULL, "
                  "details CLOB, "
                  "dueDate TEXT, "
                  "paidDate TEXT, "
                  "entityType INTEGER NOT NULL, " //0(company) or 1(person)
                  "entityID INTEGER NOT NULL, "
                  "addressID INTEGER, "
                  "status INTEGER, "
                  "FOREIGN KEY (addressID) REFERENCES Address(id)"
                  ")");

    if(!query.exec())
        qWarning("Could not create Invoice table");


    query.prepare("CREATE TABLE IF NOT EXISTS Receipt "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, "
                  "rDate TEXT NOT NULL, "
                  "details CLOB, "
                  "invoice INTEGER, "
                  "FOREIGN KEY (invoice) REFERENCES Invoice(id) "
                  ")");

    if(!query.exec())
        qWarning("Could not create Receipt table");


    query.prepare("CREATE TABLE IF NOT EXISTS Expense "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, "
                  "eDate TEXT NOT NULL, "
                  "quantity INTEGER NOT NULL CHECK (quantity >= 0), "
                  "amount INTEGER NOT NULL CHECK (amount >= 0), "
                  "gst INTEGER NOT NULL, "
                  "name VARCHAR(80) NOT NULL, "
                  "description CLOB, "
                  "gstInc INTEGER NOT NULL "
                  ")");

    if(!query.exec())
        qWarning("Could not create Expense table");


    query.prepare("CREATE TABLE IF NOT EXISTS MaterialsOnQuote "
                  "(id INTEGER PRIMARY KEY UNIQUE NOT NULL, "
                  "quoteID INTEGER NOT NULL ,"
                  "name VARCHAR(50) NOT NULL, "
                  "brand VARCHAR(50) NOT NULL, "
                  "quantity INTEGER NOT NULL, "
                  "price DOUBLE NOT NULL, "
                  "FOREIGN KEY (quoteID) REFERENCES Quote(id)"
                  ")");

    if(!query.exec())
        qWarning("Could not create Material table");


    query.prepare("CREATE TABLE IF NOT EXISTS LabourOnQuote "
                  "(id INTEGER PRIMARY KEY UNIQUE NOT NULL, "
                  "quoteID INTEGER NOT NULL ,"
                  "name VARCHAR(80) NOT NULL, "
                  "description CLOB, "
                  "hours INTEGER NOT NULL, "
                  "rate DOUBLE NOT NULL, "
                  "FOREIGN KEY (quoteID) REFERENCES Quote(id)"
                  ")");

    if(!query.exec())
        qWarning("Could not create Labour table");

    query.prepare("CREATE TABLE IF NOT EXISTS MaterialsOnInvoice "
                  "(id INTEGER PRIMARY KEY UNIQUE NOT NULL, "
                  "invoiceID INTEGER NOT NULL ,"
                  "name VARCHAR(50) NOT NULL, "
                  "brand VARCHAR(50) NOT NULL, "
                  "quantity INTEGER NOT NULL, "
                  "price DOUBLE NOT NULL, "
                  "FOREIGN KEY (invoiceID) REFERENCES Invoice(id)"
                  ")");

    if(!query.exec())
        qWarning("Could not create MaterialOnInvoice table");


    query.prepare("CREATE TABLE IF NOT EXISTS LabourOnInvoice "
                  "(id INTEGER PRIMARY KEY UNIQUE NOT NULL, "
                  "invoiceID INTEGER NOT NULL ,"
                  "name VARCHAR(80) NOT NULL, "
                  "description CLOB, "
                  "hours INTEGER NOT NULL, "
                  "rate DOUBLE NOT NULL, "
                  "FOREIGN KEY (invoiceID) REFERENCES Invoice(id)"
                  ")");

    if(!query.exec())
        qWarning("Could not create LabourOnInvoice table");


    query.prepare("CREATE TABLE IF NOT EXISTS MaterialPrices "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, "
                  "name VARCHAR(80) NOT NULL, "
                  "brand VARCHAR(80), "
                  "description CLOB, "
                  "price DOUBLE NOT NULL, "
                  "quantity INTEGER NOT NULL, "
                  "partNum VARCHAR(40), "
                  "wholesalePrice INTEGER NOT NULL"
                  ")");

    if(!query.exec())
        qWarning("Could not create MaterialPrices table");


    query.prepare("CREATE TABLE IF NOT EXISTS LabourRates "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, "
                  "name VARCHAR(80) NOT NULL, "
                  "description CLOB, "
                  "rate DOUBLE NOT NULL "
                  ")");

    if(!query.exec())
        qWarning("Could not create LabourRates table");


    query.prepare("CREATE TABLE IF NOT EXISTS Owner "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, "
                  "fName VARCHAR(80) NOT NULL, "
                  "lName VARCHAR(80) NOT NULL, "
                  "bName VARCHAR(80) NOT NULL, "
                  "abn VARCHAR(20), "
                  "mobile VARCHAR(20), "
                  "phoneNum VARCHAR(20), "
                  "faxNum VARCHAR(20), "
                  "email VARCHAR(80), "
                  "webSite VARCHAR(50), "
                  "unitNum INTEGER, "
                  "streetNum INTEGER, "
                  "street VARCHAR(80), "
                  "suburb VARCHAR(80), "
                  "city VARCHAR(80), "
                  "state INTEGER NOT NULL, "
                  "postcode INTEGER, "
                  "dbVersion VARCHAR(20), "
                  "quoteHeader TEXT, "
                  "quoteFooter TEXT, "
                  "invoiceHeader TEXT, "
                  "invoiceFooter TEXT, "
                  "logoImage BLOB, "
                  "FOREIGN KEY (state) REFERENCES State(id)"
                  ")");

    if(!query.exec())
        qWarning("Could not create Owner table");

    query.prepare("CREATE TABLE IF NOT EXISTS AddressOfPerson "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, "
                  "address INTEGER NOT NULL, "
                  "person INTEGER NOT NULL, "
                  "FOREIGN KEY (address) REFERENCES Address(id), "
                  "FOREIGN KEY (person) REFERENCES Person(id)"
                  ")");

    if(!query.exec())
        qWarning("Could not create AddressOfPerson table");


    query.prepare("CREATE TABLE IF NOT EXISTS AddressOfCompany "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE NOT NULL, "
                  "address INTEGER NOT NULL, "
                  "company INTEGER NOT NULL, "
                  "FOREIGN KEY (address) REFERENCES Address(id), "
                  "FOREIGN KEY (company) REFERENCES Company(id)"
                  ")");

    if(!query.exec())
        qWarning("Could not create AddressOfCompany table");


    query.prepare("INSERT INTO state VALUES(NULL, 'Australian Capital Territory', 'ACT')");
    query.exec();
    query.prepare("INSERT INTO state VALUES(NULL, 'New South Wales', 'NSW')");
    query.exec();
    query.prepare("INSERT INTO state VALUES(NULL, 'Northern Territory', 'NT')");
    query.exec();
    query.prepare("INSERT INTO state VALUES(NULL, 'Queensland', 'QLD')");
    query.exec();
    query.prepare("INSERT INTO state VALUES(NULL, 'South Australia', 'SA')");
    query.exec();
    query.prepare("INSERT INTO state VALUES(NULL, 'Tasmania', 'TAS')");
    query.exec();
    query.prepare("INSERT INTO state VALUES(NULL, 'Victoria', 'VIC')");
    query.exec();
    query.prepare("INSERT INTO state VALUES(NULL, 'Western Australia', 'WA')");
    query.exec();

    return true;
}
