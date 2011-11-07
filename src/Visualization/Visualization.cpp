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
#include <QProgressBar>
#include <QMessageBox>
#include <QLabel>
#include <QProgressBar>
#include <QMdiSubWindow>

#include <QSqlDatabase>
#include <QSqlError>

#include "TableEditor.h"
#include "Chart_ParallelCoordinates.h"
#include "Visualization.h"


const QString sConnectionName = "Database.db"; //! @todo Calculate the connection name
const QSize DefaultWindowSize(400,300);

using namespace std;



Visualization::Visualization(QWidget *parent, Qt::WFlags flags)
   : QMainWindow(parent, flags)
   , _map()
   , m_viewPC(NULL)
   , m_viewTable(NULL)
   , m_nNextFlightNum(0)
{
   ui.setupUi(this);

   m_dataMgmt.Connect( sConnectionName );
   this->addDockWidget(Qt::LeftDockWidgetArea, &m_dockWidgetAttr);
   m_attrSel.SetDataMgmt( &m_dataMgmt );
   m_dockWidgetAttr.SetModel( &m_attrSel );

   // -------------------------------------------------------------------------
   // Basic application function connections
   connect( ui.actionExit, SIGNAL(triggered()), this, SLOT(close()) );
   connect( ui.actionOpen, SIGNAL(triggered()), this, SLOT(LoadFile()) );
   // -------------------------------------------------------------------------

   // -------------------------------------------------------------------------
   // Visualization connections
   connect
      ( ui.actionParallel_Coordinates, SIGNAL(triggered())
      , this,                          SLOT(OnViewParallelCoordinates()) );
   // -------------------------------------------------------------------------

   // -------------------------------------------------------------------------
   // Connect the Table View for the data.
   connect( ui.actionTable, SIGNAL(triggered()), this, SLOT(OnViewTable()) );
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
   // Create a file open dialog and prompt the user for a file.
   QStringList sFilenames = QFileDialog::getOpenFileNames(this,
      tr("Open CSV"), "", tr("CSV Files (*.csv)") );

   // Prepare to process the data from the queue.
   if( !m_dataMgmt.isRunning() )
   {
      m_dataMgmt.start();
   }

   // If a file was selected then create a database for the 
   for( int s = 0; s < sFilenames.size(); ++s )
   {
      QString sFlightName;
      sFlightName.sprintf("Flight_%03d", ++m_nNextFlightNum);


      Parser::CsvParser* parser = new Parser::CsvParser;
      parser->SetParseInformation( sFilenames.at(s), &m_dataMgmt, sFlightName, sConnectionName );

      // Set the progress bar and a placeholder in the tree widget.
      QProgressBar* progress = new QProgressBar();
      m_dockWidgetAttr.CreatePlaceHolder( sFlightName, progress );

      // Connect the progress bar to the status updates from the CSV parsing thread.
      connect
         ( parser,       SIGNAL(setProgressRange(int,int))
         , progress,     SLOT(setRange(int,int)) );
      connect
         ( parser,       SIGNAL(setCurrentProgress(int))
         , progress,     SLOT(setValue(int)) );
      connect
         ( parser,       SIGNAL(finished())
         , this,         SLOT(CsvFileDone()) );

      parser->start();
   }
}

void Visualization::CsvFileDone()
{
   Parser::CsvParser* parser = dynamic_cast<Parser::CsvParser*>(QObject::sender());

   if( !parser )
   {
      std::cerr << "Error accessing flight data after load" << std::endl;
      return;
   }

   // -------------------------------------------------------------------------
   // Setup the attributes tree.
   // -------------------------------------------------------------------------
   m_dockWidgetAttr.PopulateTree(parser->GetFlightName(), &m_dataMgmt);
   // -------------------------------------------------------------------------


   // ---------------------------------------------------------------------------
   // Example widget appearing in the main window as a sub-window of the 
   // visualization
   createVisualizationUI();

   // ---------------------------------------------------------------------------
   // Clean up the parser.
   delete parser;
}

// This function is where we should set up the widgets we will use.
void Visualization::createVisualizationUI()
{
   _map = new MapWidget(this);
   _mapsubwindow = ui.mdiArea->addSubWindow(_map);
   _mapsubwindow->resize(DefaultWindowSize);
   _mapsubwindow->show();

   //subwindow->resize(DefaultWindowSize);
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
   const Data::Selections& selections = m_attrSel.GetSelectedAttributes();
   for( Data::Selections::iterator i = selections.begin(); i != selections.end(); ++i )
   {
      m_viewTable = new TableEditor(sConnectionName, i.key());
      m_viewTable->setObjectName(QString::fromUtf8("chart"));
      m_viewTable->setWindowTitle(QApplication::translate("VisualizationClass", qPrintable(i.key()), 0, QApplication::UnicodeUTF8));
      QMdiSubWindow* subwindow = ui.mdiArea->addSubWindow(m_viewTable);
      subwindow->setAttribute( Qt::WA_DeleteOnClose );
      subwindow->resize(DefaultWindowSize);
      subwindow->show();
   }
}


void Visualization::OnViewParallelCoordinates()
{
   m_viewPC = new Chart::ParallelCoordinates(&m_attrSel);
   m_viewPC->setObjectName(QString::fromUtf8("chart"));
   m_viewPC->setWindowTitle(QApplication::translate("VisualizationClass", "Chart", 0, QApplication::UnicodeUTF8));
   QMdiSubWindow* subwindow = ui.mdiArea->addSubWindow(m_viewPC);
   subwindow->setAttribute( Qt::WA_DeleteOnClose );
   subwindow->resize(DefaultWindowSize);
   subwindow->show();
}
