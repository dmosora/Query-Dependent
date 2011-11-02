// Written by David Sheets
// Visualization product for analyzing data, flight data in particular.
// Copyright (C) 2011  David Sheets (dsheets4@kent.edu)
//
// Visualization is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <iostream>

#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QProgressBar>
#include <QMdiSubWindow>

#include <QSqlDatabase>
#include <QSqlError>

#include "TableEditor.h"
#include "Chart_ParallelCoordinates.h"
#include "Visualization.h"


const QString sDataSetName    = "DataName";    //! @todo Calculate the database name
const QString sConnectionName = "DataName.db"; //! @todo Calculate the connection name


using namespace std;



Visualization::Visualization(QWidget *parent, Qt::WFlags flags)
   : QMainWindow(parent, flags)
   , _map()
   , m_progress(NULL)
   , m_viewPC(NULL)
   , m_viewTable(NULL)
{
   ui.setupUi(this);

   this->addDockWidget(Qt::LeftDockWidgetArea, &m_dockWidgetAttr);

   // -------------------------------------------------------------------------
   // Basic application function connections
   connect( ui.actionExit, SIGNAL(triggered()), this, SLOT(close()) );
   connect( ui.actionOpen, SIGNAL(triggered()), this, SLOT(LoadFile()) );
   // -------------------------------------------------------------------------

   // -------------------------------------------------------------------------
   // CSV Parser connections
   connect( &m_csvParser, SIGNAL(finished()),   this, SLOT(CsvFileDone()) );
   // -------------------------------------------------------------------------

   // -------------------------------------------------------------------------
   // Visualization connections
   connect
      ( ui.actionParallel_Coordinates, SIGNAL(triggered())
      , this,                          SLOT(OnViewParallelCoordinates()) );
   // -------------------------------------------------------------------------
}

Visualization::~Visualization()
{

}

void Visualization::closeEvent( QCloseEvent* event )
{
   m_csvParser.stopParse();
}

void Visualization::LoadFile()
{
   // If a file is currently being parsed then indicate that the user must 
   // wait for it to finish.
   if( m_csvParser.isRunning() )
   {
      QMessageBox message;
      message.setText(tr("The previously selected file is still loading!"
         "Please wait for it to complete."));
      message.exec();

      return;
   }

   // Create a file open dialog and prompt the user for a file.
   QString sFilename = QFileDialog::getOpenFileName(this,
      tr("Open CSV"), "", tr("CSV Files (*.csv)") );

   // If a file was selected then create a database for the 
   if( !sFilename.isEmpty() )
   {
      QFile file;
      m_dataMgmt.Connect( sConnectionName, sDataSetName );
      m_csvParser.SetParseInformation( sFilename, &m_dataMgmt, sDataSetName, sConnectionName );

      if( !m_progress )
      {
         QLabel* label = new QLabel(tr("Loading ") + sDataSetName);
         m_progress = new QProgressBar();
         ui.statusBar->addWidget(label);
         ui.statusBar->addWidget(m_progress);
      }

      // Connect the progress bar to the status updates from the CSV parsing thread.
      connect
         ( &m_csvParser,      SIGNAL(setProgressRange(int,int))
         , m_progress,        SLOT(setRange(int,int)) );
      connect
         ( &m_csvParser,      SIGNAL(setCurrentProgress(int))
         , m_progress,        SLOT(setValue(int)) );

      m_csvParser.start();
   }
}

void Visualization::CsvFileDone()
{
   // -------------------------------------------------------------------------
   // Setup the attributes tree.
   // -------------------------------------------------------------------------
   m_dockWidgetAttr.PopulateTree(&m_dataMgmt, sDataSetName);
   m_dockWidgetAttr.SetModel( &m_attrSel );
   // -------------------------------------------------------------------------

   // -------------------------------------------------------------------------
   // Connect the Table View for the data.
   connect( ui.actionTable, SIGNAL(triggered()), this, SLOT(OnViewTable()) );
   // ---------------------------------------------------------------------------

   // ---------------------------------------------------------------------------
   // Setup the attributes tree.
   // ---------------------------------------------------------------------------

   // ---------------------------------------------------------------------------
   // Example widget appearing in the main window as a sub-window of the 
   // visualization
   createVisualizationUI();

   // ---------------------------------------------------------------------------
}

// This function is where we should set up the widgets we will use.
void Visualization::createVisualizationUI()
{
    _map = new MapWidget(this);
    _mapsubwindow = ui.mdiArea->addSubWindow(_map);
    _mapsubwindow->resize(400,300);
    _mapsubwindow->show();

   //subwindow->resize(400,300);
   //subwindow->show();
   //subwindow->showMaximized();
}

void Visualization::CsvFileStatus(bool bSuccess, QString sFilename)
{
   int tmpBrk = 1;
}

void Visualization::OnViewTable()
{
   //! @todo This might be a memory leak.  Determine the DeleteOnClose 
   //!       property.  In the long run, this code will need redone to 
   //!       connect the chart(s) up to dynamically updates based on 
   //!       user selections.
   m_viewTable = new TableEditor(sConnectionName, sDataSetName);
   m_viewTable->setObjectName(QString::fromUtf8("chart"));
   m_viewTable->setWindowTitle(QApplication::translate("VisualizationClass", "Chart", 0, QApplication::UnicodeUTF8));
   QMdiSubWindow* subwindow = ui.mdiArea->addSubWindow(m_viewTable);
   subwindow->showMaximized();
}


void Visualization::OnViewParallelCoordinates()
{
   // ---------------------------------------------------------------------------
   // Widget appearing in the main window as a sub-window of the visualization
   // ---------------------------------------------------------------------------
   m_viewPC = new Chart::ParallelCoordinates(sConnectionName, sDataSetName, &m_attrSel);
   m_viewPC->setObjectName(QString::fromUtf8("chart"));
   m_viewPC->setWindowTitle(QApplication::translate("VisualizationClass", "Chart", 0, QApplication::UnicodeUTF8));
   QMdiSubWindow* subwindow = ui.mdiArea->addSubWindow(m_viewPC);
   subwindow->showMaximized();
}
