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

#include "EventDetector.h"
#include "TableEditor.h"
#include "Chart_ParallelCoordinates.h"
#include "seansGlyphCode\EventGlyph.h"
#include "Visualization.h"


//const QString sConnectionName = "Database.db";
const QString sConnectionName = ":memory:";
const QSize DefaultWindowSize(400,300);

using namespace std;



Visualization::Visualization(QWidget *parent, Qt::WFlags flags)
   : QMainWindow(parent, flags)
   , _map()
   , m_viewPC(NULL)
   , m_viewTable(NULL)
   , m_nNextFlightNum(0)
   , m_nToComplete(0)
{
   ui.setupUi(this);

   m_dataMgmt.Connect( sConnectionName );
   this->addDockWidget(Qt::LeftDockWidgetArea, &m_dockWidgetAttr);
   m_attrSel.SetDataMgmt( &m_dataMgmt );
   m_dockWidgetAttr.SetModel( &m_attrSel );
   
   QLabel* label = new QLabel(tr("Data Commit: "));
   QProgressBar* progress = new QProgressBar();
   ui.statusBar->addWidget(label);
   ui.statusBar->addWidget(progress);

   // -------------------------------------------------------------------------
   // Basic application function connections
   connect( ui.actionExit, SIGNAL(triggered()), this, SLOT(close()) );
   connect( ui.actionOpen, SIGNAL(triggered()), this, SLOT(LoadFile()) );
   // -------------------------------------------------------------------------

   // -------------------------------------------------------------------------
   // Data loading signals.
   connect
      ( &m_dataMgmt,  SIGNAL(FlightComplete(QString))
      , this,         SLOT(DatabaseStatus(QString)) );
   connect
      ( &m_dataMgmt,  SIGNAL(setProgressRange(int,int))
      , progress,     SLOT(setRange(int,int)) );
   connect
      ( &m_dataMgmt,  SIGNAL(setCurrentProgress(int))
      , progress,     SLOT(setValue(int)) );
   // -------------------------------------------------------------------------

   // -------------------------------------------------------------------------
   // Visualization connections
   connect
      ( ui.actionParallel_Coordinates, SIGNAL(triggered())
      , this,                          SLOT(OnViewParallelCoordinates()) );
   connect
      ( ui.actionEvent_Glyph,          SIGNAL(triggered())
      , this,                          SLOT(OnViewEventGlyph()) );
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
   m_listFileNames = QFileDialog::getOpenFileNames(this,
      tr("Open CSV"), "../../../data", tr("CSV Files (*.csv)") );

   // Initiate the load for the selected files but limit the number of threads
   // that will be launched at one time.
   int nLaunched = 0;
   int nThreads = QThread::idealThreadCount()-1;
   if( nThreads < 1 )
   {
      nThreads = 1;
   }
   for( int s = m_listFileNames.size()-1; s >= 0 && nLaunched < nThreads; --s )
   {
      LoadFlight( m_listFileNames.takeLast() );
      ++nLaunched;
   }
}

void Visualization::LoadFlight( const QString& filename )
{
   // Prepare to process the data from the queue.
   if( !m_dataMgmt.isRunning() )
   {
      m_dataMgmt.start();
   }

   // Extract the flight name from the filename.
   QString sFlightName;
   int index = filename.lastIndexOf(QDir::separator());
   if( index != -1 )
   {
      index = filename.length() - index - 1;
      sFlightName = filename.right(index);
      index = sFlightName.lastIndexOf('.');
      if( index != -1 )
      {
         sFlightName = sFlightName.left(index);
      }
      sFlightName.replace(' ', '_');
   }
   else
   {
      sFlightName.sprintf("Default_%03d", ++m_nNextFlightNum);
   }

   // Create and setup a new parser thread.
   Parser::CsvParser* parser = new Parser::CsvParser;
   parser->SetParseInformation( filename, &m_dataMgmt, sFlightName, sConnectionName );

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

   // Connect the CSV parser so we can cleanup the memory.
   connect
      ( parser,       SIGNAL(finished())
      , this,         SLOT(CsvFileDone()) );

   parser->start();

   // Increment when the CSV thread is started.  
   // Decrement when the Database is done
   ++m_nToComplete;
}

void Visualization::CsvFileDone()
{
   // ---------------------------------------------------------------------------
   // Clean up the old parser.
   Parser::CsvParser* parser = dynamic_cast<Parser::CsvParser*>(QObject::sender());
   if( !parser )
   {
      std::cerr << "Error accessing flight data after load" << std::endl;
      return;
   }
   delete parser;
   
   // ---------------------------------------------------------------------------
   // If there are other files in the queue, begin loading them.
   if( !m_listFileNames.empty() )
   {
      LoadFlight( m_listFileNames.takeFirst() );
   }
}

void Visualization::DatabaseStatus(QString sFlightName)
{
   // -------------------------------------------------------------------------
   // Detect the events for the flight.
   // -------------------------------------------------------------------------
   Event::EventDetector evtDetect;
   Data::EventData evtData;
   if( evtDetect.DetectEvents( sFlightName, &m_dataMgmt, evtData) )
   {
      m_dataMgmt.SetEventData( sFlightName, evtData );
   }

   // -------------------------------------------------------------------------
   // Setup the attributes tree.
   // -------------------------------------------------------------------------
   m_dockWidgetAttr.PopulateTree(sFlightName, &m_dataMgmt);
   // -------------------------------------------------------------------------


   // -------------------------------------------------------------------------
   // Example widget appearing in the main window as a sub-window of the 
   // visualization
   createVisualizationUI();
   
   // Increment when the CSV thread is started.  
   // Decrement when the Database is done
   --m_nToComplete;

   if( m_nToComplete == 0 )
   {
      cout << "Flight processing complete. Stopping data management thread." << endl;
      m_dataMgmt.stopProcessing();
   }
}

// This function is where we should set up the widgets we will use.
void Visualization::createVisualizationUI()
{
   _map = new MapWidget(this);
   _map->resize(ui.mdiArea->size());
   _map->updateMap();

   // Prepare the subwindow in the Mdi
   _mapsubwindow = ui.mdiArea->addSubWindow(_map);
   _mapsubwindow->resize(ui.mdiArea->size());
   _mapsubwindow->setMaximumSize(533,533);

   // Make this add a GraphicsView instead of the widget
   QGraphicsView* view = new QGraphicsView(_map);
   view->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
   view->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
   _map->setMapView(view);

   // Show the views necessary
   view->show();
   _mapsubwindow->show();

   //subwindow->resize(DefaultWindowSize);
   //subwindow->show();
   //subwindow->showMaximized();
}

void Visualization::OnViewTable()
{
   const Data::Selections& selections = m_attrSel.GetSelectedAttributes();
   for( Data::Selections::const_iterator i = selections.begin(); i != selections.end(); ++i )
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

void split_str(const std::string& str, std::vector<float>& vec, const char& split_char)
{
	std::string temp_str;
	unsigned int i = 0;
	unsigned int j = 0;
	while(i < str.length())
	{
		j=i;
		while( (j<str.length()) && str[j] != '\n' && str[j] != split_char)
		{
			++j;
		}
		temp_str = str.substr(i, j-i);
		if(temp_str.length() > 0)
			vec.push_back(atof(temp_str.c_str()));
		if(i==j)
			++i;
		else
			i=j;
	}
}
void Visualization::OnViewEventGlyph()
{
   EventGlyph* event_glyph = new EventGlyph(700, 700, 171);
   event_glyph->SetAttributeNumber(6);

   std::vector<float> data;
   
   const Data::EventDatabase& evtDb = m_dataMgmt.GetEventData();
   Data::EventDatabaseIterator iDb(evtDb);
   while( iDb.hasNext() )
   {
      iDb.next();
      data.clear();
      Data::EventDataIterator iList(iDb.value());
      while( iList.hasNext() )
      {
         data.push_back( iList.next()._valueNormal.toFloat() );
      }
      event_glyph->CreatePointSet(data, iDb.key().toStdString());
   }
   event_glyph->ShowGlyph();

   QMdiSubWindow* subwindow = ui.mdiArea->addSubWindow(event_glyph);
   subwindow->setAttribute( Qt::WA_DeleteOnClose );
   subwindow->resize(DefaultWindowSize);
   subwindow->show();
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
