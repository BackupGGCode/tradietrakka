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

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QSettings>

#define ORGNAME "TradieTrakka"
#define APPNAME "TradieTrakka"

/**
  * Class to read and write application settings
  * to the app configuration file which is stored
  * in a platform independent way.
  */
class AppSettings : public QObject
{
    QSettings *settings;
public:
    AppSettings();
    ~AppSettings();

    bool hasDbSet();
    QString getDbName();
    QString getDbPath();

    QByteArray getGeometry();
    QString getOrgName();

    void setDbName(const QString &dbName);
    void setDbPath(const QString &dbPath);

    void setGeometry(const QByteArray &byteArray);
    void setOrgName(const QString &orgName);

    bool hasDbaseSettings();
};

#endif // APPSETTINGS_H
