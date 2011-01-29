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

#include "appsettings.h"

//AppSettings::AppSettings(const QString &orgName, const QString &appName)
AppSettings::AppSettings()
{
//    this->settings = new QSettings(orgName, appName, this);
    this->settings = new QSettings(ORGNAME, APPNAME, this);
}

AppSettings::~AppSettings()
{
    delete settings;
}

/**
  * Check to see if database has already been set.
  * @returns True is database setting exits, False if not
  */
bool AppSettings::hasDbSet()
{
    if(settings->contains("Database/dbName"))
        return true;
    return false;
}

/**
  * Retrieve Database file name
  * @return QString the db file name
  */
QString AppSettings::getDbName()
{
    return settings->value("Database/dbName").toString();
}

QString AppSettings::getDbPath()
{
    return settings->value("Database/dbPath").toString();
}

void AppSettings::setDbPath(const QString &dbPath)
{
    settings->setValue("Database/dbPath", dbPath);
}



/**
  * Retrieve App geometry
  * @returns The main window size and location
  */
QByteArray AppSettings::getGeometry()
{
    return settings->value("MainWindow/geometry").toByteArray();
}

/**
  * Retrieve the Organisation Name
  * @return QString orgName
  */
QString AppSettings::getOrgName()
{
    return settings->value("General/orgName").toString();
}

/**
  * Save Database name
  * @param QString dbName The Database file Name
  */
void AppSettings::setDbName(const QString &dbName)
{
    settings->setValue("Database/dbName", dbName);
}

/**
  * Save the Mainwindow size and location
  * @param QByteArray byteArray The Window geometry
  */
void AppSettings::setGeometry(const QByteArray &byteArray)
{
    settings->setValue("MainWindow/geometry", byteArray);
}

/**
  * Save the Organisation name
  * @param QString The Organisation name
  */
void AppSettings::setOrgName(const QString &orgName)
{
    settings->setValue("General/companyName", orgName);
}

bool AppSettings::hasDbaseSettings()
{
    if(settings->contains("Database/dbName"))
        return true;
    return false;
}
