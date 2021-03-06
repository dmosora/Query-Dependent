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
#include "seansGlyphCode/EventGlyph.h"
#include "Visualization.h"
#include "DataNormalizer.h"


//const QString sConnectionName = "Database.db";
const QString sConnectionName = ":memory:";
const QSize DefaultWindowSize(600,450);

using namespace std;


Visualization::Visualization(QWidget *parent, Qt::WFlags flags)
   : QMainWindow(parent, flags)
   , _map(0)
   , m_viewPC(NULL)
   , m_viewTable(NULL)
   , m_nNextFlightNum(0)
   , m_nToComplete(0)
{
   ui.setupUi(this);


   Event::EventDetector evtDetect;
   m_dataMgmt.Connect( sConnectionName );
   m_dataMgmt.SetEventDefinition( evtDetect.GetEventDefinition() );
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
   connect
      ( ui.actionReal_Time_Glyph,      SIGNAL(triggered())
       , this,                         SLOT(OnViewRealTimeGlyph()) );
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
      //cout << "Starting data management thread." << endl;
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
   //! @todo After thinking some this is probably dangerous to delete the sender
   //!       object because if there are other objects registered for this signal
   //!       it probably won't get handled properly.  Something to work on later.
   //!       For now, the object is created internal to this one and it's the only
   //!       one listening for signals from the object.
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
   // Creates the necessary map if it does not yet exist properly,
   // otherwise add new flights to the vis
   createVisualizationUI();
   
   // Increment when the CSV thread is started.  
   // Decrement when the Database is done
   --m_nToComplete;

   if( m_nToComplete == 0 )
   {
      //cout << "Flight processing complete. Stopping data management thread." << endl;
      m_dataMgmt.stopProcessing();
   }
}

// Sets up the main map view and widgets associated with it
void Visualization::createVisualizationUI()
{
    if(!_map) {
        _map = new MapWidget(this);
        _map->resize(ui.mdiArea->size());
        //_map->updateMap();                 // Dynamic map ONLY
        _map->setDataMgmt(&m_dataMgmt);

        // Make the time slider for this
        _toolbar = new TimeSlider(this);
        _toolbar->setMovable(false);

        // Organize them in a layout and make the mdi widget
        _mapAndSlider = new QVBoxLayout(this);
        _mapAndSlider->addWidget(_toolbar);
        _mapAndSlider->addWidget(_map);

        _mdiMapWidget = new QWidget(this);
        _mdiMapWidget->setLayout(_mapAndSlider);

        // Prepare the subwindow in the Mdi
        //_mapsubwindow = ui.mdiArea->addSubWindow(_map);
        _mapsubwindow = ui.mdiArea->addSubWindow(_mdiMapWidget);
        _mapsubwindow->resize(607,550);
        _mapsubwindow->setMaximumSize(_mapsubwindow->size());
        _mapsubwindow->setMinimumSize(_mapsubwindow->size());

        // Make this add a GraphicsView instead of the widget
        QGraphicsView* view = new QGraphicsView(_map);
        view->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
        view->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
        _map->setMapView(view);
        _map->setTimeSlider(_toolbar);

        // Add the comboBox and populate it
        _loadedFlights = new QComboBox(this);
        QStringList flights;
        m_dataMgmt.GetLoadedFlights(flights);

        // Add items one at a time so we can color code them
        for(int i = 0; i < flights.size();i++) {
            QPixmap pic(40,40);
            pic.fill(QColor::fromHsv((100 * i) % 360,255,230,150));
            QIcon icon(pic);
            _loadedFlights->addItem(icon,flights[i]);
        }

        _toolbar->addWidget(_loadedFlights);
        _map->setActiveFlight(flights.at(0));

        // Finish setting up the first range of the slider
        QStringList temp;
        Data::Buffer buffer;
        m_dataMgmt.GetDataAttributes(flights.at(0), temp, buffer);
        _toolbar->setNewMax(buffer._params.size());

        // Get the first set of data
        _map->getNewAttributes();

        // Show the views necessary
        view->show();
        _mapsubwindow->show();

        // Make connections for the toolbar
        connect(_toolbar, SIGNAL(timeChanged(int)), _map, SLOT(onTimeChanged(int)));
        connect(_loadedFlights,SIGNAL(currentIndexChanged(QString)),_map,SLOT(onActiveFlightChanged(QString)));
        connect(_loadedFlights,SIGNAL(currentIndexChanged(int)),_map,SLOT(onActiveFlightIndexChanged(int)));

        // Maximize the window, it's too small!
        showMaximized();
    } else {
        /// This could be greatly improved by not going through the flights already loaded
        // Add new flights to the map vis
        int currentItem = _loadedFlights->currentIndex();
        _loadedFlights->clear();

        QStringList flights;
        m_dataMgmt.GetLoadedFlights(flights);

        // Add items one at a time so we can color code them
        for(int i = 0; i < flights.size();i++) {
            QPixmap pic(40,40);
            pic.fill(QColor::fromHsv((100 * (i)) % 360,255,230,200));
            QIcon icon(pic);
            _loadedFlights->addItem(icon,flights[i]);
        }

        // Update the slider's size to the max
        for(int i = 0; i < flights.size();i++) {
            QStringList temp;
            Data::Buffer buffer;
            m_dataMgmt.GetDataAttributes(flights.at(i), temp, buffer);
            _toolbar->setNewMax(buffer._params.size());
        }

        _map->getNewAttributes();

        // Reset the index to what they had selected before
        _loadedFlights->setCurrentIndex(currentItem);

        // Draw the new paths
        _map->updateMap();
    }
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
   EventGlyph* event_glyph = new EventGlyph(DefaultWindowSize.width(), DefaultWindowSize.height()-30, 6);
   //EventGlyph* event_glyph = new EventGlyph(700, 700, 6);

   const Data::EventDatabase& evtDb = m_dataMgmt.GetEventData();


   // Set the max lines data.
   event_glyph->SetMaxLines( evtDb._def._maxValues );
   event_glyph->SetMinLines( evtDb._def._minValues );
   event_glyph->SetAxisLabels( evtDb._def._labels );

   // Set the event data
   std::vector<float> data;
   Data::EventContainerIterator iDb(evtDb._events);
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
   //event_glyph->ShowGlyph();

   QMdiSubWindow* subwindow = ui.mdiArea->addSubWindow(event_glyph->GetGlyphView());
   subwindow->setWindowTitle(QApplication::translate("VisualizationClass", "Event Glyph", 0, QApplication::UnicodeUTF8));
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

// Possible problem with this implementation:
// If a person changes the attributes selected, we need to
//     retreieve another buffer on time changed
void Visualization::OnViewRealTimeGlyph()
{
    QStringList flights;
    m_dataMgmt.GetLoadedFlights(flights);

    // Create a buffer for each of the currently loaded flights for switching
    for(int i = 0; i < flights.size(); i++) {
        // Get the attribute labels that are selected
        Data::Selections atts = m_attrSel.GetSelectedAttributes();
        QMap<QString, QStringList>::const_iterator iter = atts.find(flights[i]);

        if( iter != atts.end() )
        {
           QStringList attNames = iter.value();

           // Get attributes' data for glyph
           Data::Buffer buffer;
           m_dataMgmt.GetDataAttributes(flights[i],attNames, buffer);
           Data::Normalizer norm;
           norm.Process(buffer);

           // Pair up the buffer with its flight_id
           //QPair<QString, Data::Buffer> pair(flights[i],buffer);

           _buffers.insert(flights[i],buffer);
        }
    }

    // Actually make a glyph and add it
    // Get the current atts selected
    Data::Selections atts = m_attrSel.GetSelectedAttributes();
    QMap<QString, QStringList>::const_iterator iter = atts.find(_loadedFlights->currentText());

    // Pull up the preloaded buffer
    QMap<QString, Data::Buffer>::iterator b_iter = _buffers.find(_loadedFlights->currentText());
    Data::Buffer buffer = b_iter.value();

    QStringList attNames = iter.value();

    // Create the object
    RealTimeGlyph* rt_glyph = new RealTimeGlyph(DefaultWindowSize.width(), DefaultWindowSize.height()-30, attNames.size());

    // Get the labels and put them in a form we can use
    std::vector<std::string> labels;
    for(int j = 0;j < attNames.size();j++) {
        labels.push_back(attNames.at(j).toStdString());
    }

    // Set the max lines data.
    rt_glyph->SetAxisLabels( labels );

    // Draw the current point set
    rt_glyph->DrawPointSet(buffer._params.at(_toolbar->slider()->value())._dataVector);

    QMdiSubWindow* subwindow = ui.mdiArea->addSubWindow(rt_glyph->GetGlyphView());
    subwindow->setWindowTitle(QApplication::translate("VisualizationClass", qPrintable(_loadedFlights->currentText()), 0, QApplication::UnicodeUTF8));
    subwindow->setAttribute( Qt::WA_DeleteOnClose );
    subwindow->resize(DefaultWindowSize);
    subwindow->setFixedSize(subwindow->width(),subwindow->height());
    subwindow->show();
    rt_glyph->GetGlyphView()->CorrectResize(subwindow->width(),subwindow->height());

    _rtGlyphs.push_back(rt_glyph);
    connect(_toolbar, SIGNAL(timeChanged(int)), SLOT(onTimeChanged(int)));
}

void Visualization::onTimeChanged(int idx)
{
    if(idx % 15 == 0) {
            Data::Selections atts = m_attrSel.GetSelectedAttributes();
            RealTimeGlyph* rt_glyph = _rtGlyphs[0];             // Should only be one in this implementation
            //EventGlyph* event_glyph = new EventGlyph(700, 700, 6);

            // Set attributes in glyph
            QMap<QString, QStringList>::const_iterator iter = atts.find(_loadedFlights->currentText());
            if( iter != atts.end() )
            {
               QStringList attNames = iter.value();
               QMap<QString, Data::Buffer>::iterator b_iter = _buffers.find(_loadedFlights->currentText());
               Data::Buffer buffer = b_iter.value();
               //m_dataMgmt.GetDataAttributes(_flights[i],attNames, buffer);
               /*QList<QVariant> stuff = _buffers.at(i)._params.at(idx)._dataVector;
               std::vector<float> attValues;
               for(int j = 0; j < stuff.size();j++) {
                   attValues.push_back(stuff[j].toDouble());
               }*/

               // Make sure the labels are current to the selection
               std::vector<std::string> labels;
               for(int j = 0;j < attNames.size();j++) {
                   labels.push_back(attNames.at(j).toStdString());
               }
               rt_glyph->SetAxisLabels( labels );

               if( idx < buffer._params.size() )
               {
                  rt_glyph->DrawPointSet(buffer._params.at(idx)._dataVector);
                  //rt_glyph->GetGlyphView()->CorrectResize();
               }
            }
    }
}
