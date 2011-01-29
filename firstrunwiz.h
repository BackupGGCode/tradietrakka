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

#ifndef FIRSTRUNWIZ_H
#define FIRSTRUNWIZ_H

#include <QWizard>
#include <QComboBox>

#include "appsettings.h"
#include "dbasectrl.h"

/**
  * Wizard for initial setup configuration.
  * Sets up Database and user preferences.
  */
class FirstRunWiz : public QWizard
{
    Q_OBJECT

    DBaseCtrl *dbaseCtrl;
    AppSettings *appSettings;
    /// Enumerated type for each page
    enum PageEnum { Intro, Database, Advanced, Complete };

    QLineEdit *dbName;           ///< Sqlite Database Name.
    QLineEdit *dbDir;            ///< Sqlite Database Directory.

public:
    FirstRunWiz(AppSettings *appSettings, DBaseCtrl *dbaseCtrl);

private:

    QWizardPage* intro();
    QWizardPage* database();
    QWizardPage* businessNameAddress();
    QWizardPage* logo();
    QWizardPage* complete();

    int nextId() const;

private slots:
    void accept();

public slots:
    void selectDir();
};

#endif // FIRSTRUNWIZ_H
