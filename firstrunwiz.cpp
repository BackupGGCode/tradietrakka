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

#include "firstrunwiz.h"
#include "settingsdialog.h"

#include <QDir>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QFileDialog>
#include <QPushButton>

FirstRunWiz::FirstRunWiz(AppSettings *appSettings, DBaseCtrl *dbaseCtrl)
{

    this->appSettings = appSettings;
    this->dbaseCtrl = dbaseCtrl;

    setWizardStyle(QWizard::ModernStyle);
    setWindowTitle("TradieTrakka settings wizard");

    setPixmap(QWizard::LogoPixmap, QPixmap(":/icons/smallLogo.png"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/icons/watermark.png"));

    // Layout of the wizard buttons.
    QList<QWizard::WizardButton> layout;
    layout  << QWizard::Stretch
            << QWizard::BackButton << QWizard::NextButton
            << QWizard::FinishButton << QWizard::CancelButton;

    setButtonLayout(layout);

    //Must be after custom layout.
    setOptions(NoBackButtonOnStartPage);

    //Set and assign pages to their enums.
    setPage(Intro, intro());
    setPage(Database, database());
    setPage(Complete, complete());

    setStartId(Intro);
}

/**
  * Creates the first page of the wizard in which we welcome the user.
  * @returns A pointer to the new page.
  */
QWizardPage* FirstRunWiz::intro()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Welcome");
    page->setSubTitle(" ");
    QLabel *welcomeLabel = new QLabel(tr("Welcome to TradieTrakka.")+"\n");

    QLabel *label = new QLabel(tr("This wizard will help you get set up "
                               "and customise TradieTrakka for your business.")+"\n\n"+
                               tr("To review and accept the defaults please click next."));
    label->setWordWrap(true);

    QGridLayout *layout = new QGridLayout(page);
    layout->addWidget(welcomeLabel);
    layout->addWidget(label);

    return page;
}

/**
  * Sets up the Sqlite Database
  * @returns A pointer to the new page.
  */
QWizardPage* FirstRunWiz::database()
{
    QWizardPage *page = new QWizardPage;

    page->setTitle("File location");
    page->setSubTitle(" ");

    QGridLayout *layout = new QGridLayout(page);

    if(appSettings->hasDbaseSettings())
    {
        dbName = new QLineEdit(appSettings->getDbName());
        dbDir = new QLineEdit(appSettings->getDbPath());
    } else {
         dbName = new QLineEdit("TradieTrakka.tt");
         dbDir = new QLineEdit(QDir::homePath());
    }
    QLabel *dbNameLabel = new QLabel(tr("File Name"));

    layout->addWidget(dbNameLabel, 0, 0);
    layout->addWidget(dbName, 0, 1);

    QLabel *dbSqliteDirLabel = new QLabel(tr("Directory"));

    QPushButton * selectDirectoryButton = new QPushButton(this);
    selectDirectoryButton->setIcon(QIcon(":/icons/folder_database.png"));

    QLabel *desc = new QLabel("\n\nTradieTrakka stores all your company\n"
                              "information in a \".tt\" file.\n\n"
                              "Please select a location where you would\n"
                              "like to keep this file or accept\n"
                              "the default location");

    layout->addWidget(dbSqliteDirLabel, 1, 0);
    layout->addWidget(dbDir, 1, 1);
    layout->addWidget(selectDirectoryButton, 1, 2);
    layout->addWidget(desc, 2, 1);

    connect(selectDirectoryButton, SIGNAL(clicked()), this, SLOT(selectDir()));

    return page;
}


/**
  * Final page with confirmation.
  * @returns A pointer to the new page.
  */
QWizardPage* FirstRunWiz::complete()
{
   QWizardPage *page = new QWizardPage;
   page->setTitle("Settings Complete");
   page->setSubTitle("Finished");

   QLabel *label = new QLabel("Click finish to save settings.\nThis will "
                              "overwrite any previously saved settings.");
   label->setWordWrap(true);

   QGridLayout *layout = new QGridLayout(page);
   layout->addWidget(label);

   return page;
}

/**
  * Overrides the accept function.
  * Called when the "Finish" button is pressed.
  * Final data processesing is done in this function.
  */
void FirstRunWiz::accept()
{
   this->close();
}

/**
  * Slot to launch a dialog that allows the user to select a directory.
  */
void FirstRunWiz::selectDir()
{
    QString dirString = QFileDialog::getExistingDirectory(this, "Select directory", QDir::homePath(),
                                                          QFileDialog::ShowDirsOnly);

    if(!dirString.isNull())
        dbDir->setText(dirString);
}

/**
  * Overrides the nextId() function for *all* of the wizard pages.
  * Actions taken, and next pages to navigate to, are handled in this function.
  * The Non-linear flow is determined in this function.
  * User set variables are also assigned here.
  * @returns The enum of the next page to navigate to.
  */
int FirstRunWiz::nextId() const
{
    switch(currentId())
    {
        case Intro:
            return Database;
        case Database:
            appSettings->setDbName(dbName->text());
            appSettings->setDbPath(dbDir->text());
            return Complete;
        case Complete:
            dbaseCtrl->connect(true);
            dbaseCtrl->createTables();
        default:
            return -1;
    }
}
