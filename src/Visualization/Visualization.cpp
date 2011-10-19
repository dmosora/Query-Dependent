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
#include "Chart.h"
#include "Visualization.h"


const QString sDataSetName    = "DataName";    //! @todo Calculate the database name
const QString sConnectionName = "DataName.db"; //! @todo Calculate the connection name

Chart* chart = NULL;

using namespace std;



Visualization::Visualization(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
	, m_chart(NULL)
        , m_progress(NULL)
        , _map()

{
	ui.setupUi(this);

   // -------------------------------------------------------------------------
   // Basic application function connections
	connect( ui.actionExit, SIGNAL(triggered()), this, SLOT(close()) );
	connect( ui.actionOpen, SIGNAL(triggered()), this, SLOT(LoadFile()) );

   // -------------------------------------------------------------------------
   // CSV Parser connections
	connect( &m_csvParser, SIGNAL(finished()),     this, SLOT(CsvFileDone()) );
	connect
      ( &m_csvParser, SIGNAL(fileDone(bool,QString))
      , this,         SLOT(CsvFileStatus(bool,QString)) );

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
	if( m_csvParser.isRunning() )
	{
		QMessageBox message;
		message.setText("The previously selected file is still loading!"
                      "Please wait for it to complete.");
		message.exec();

		return;
	}

	QString sFilename = QFileDialog::getOpenFileName(this,
		tr("Open CSV"), "", tr("CSV Files (*.csv)") );

	if( !sFilename.isEmpty() )
	{
		QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", sConnectionName);
		db.setDatabaseName(sConnectionName);
		if (!db.open())
		{
			cerr << "Failed to open the flight database for connection: " 
				  << qPrintable(sDataSetName) << endl;
			cerr << "Error Message: " << qPrintable(db.lastError().text()) << endl;

			return;
		}


		QFile file;
		m_csvParser.SetParseInformation( sFilename, sDataSetName, sConnectionName );
		
      if( !m_progress )
      {
		   QLabel* label = new QLabel("Loading " + sDataSetName);
		   m_progress = new QProgressBar();
		   ui.statusBar->addWidget(label);
		   ui.statusBar->addWidget(m_progress);
      }
      
      // Connect the progress bar to the status updates from the CSV parsing thread.
	   connect
         ( &m_csvParser, SIGNAL(setRange(int,int))
         , m_progress,   SLOT(setRange(int,int)) );
	   connect
         ( &m_csvParser, SIGNAL(setValue(int))
         , m_progress,   SLOT(setValue(int)) );

		m_csvParser.start();
	}

        createVisualizationUI();
}

void Visualization::CsvFileDone()
{
	// ---------------------------------------------------------------------------
   // Connect the Table View for the data.
	connect( ui.actionTable, SIGNAL(triggered()), this, SLOT(OnViewTable()) );
	// ---------------------------------------------------------------------------
   
	// ---------------------------------------------------------------------------
   // Setup the attributes tree.
	// ---------------------------------------------------------------------------

	// ---------------------------------------------------------------------------
   // Example widget appearing in the main window as a sub-window of the 
   // visualization
   m_chart = new Chart(sConnectionName, sDataSetName);
   m_chart->setObjectName(QString::fromUtf8("chart"));
   m_chart->setWindowTitle(QApplication::translate("VisualizationClass", "Chart", 0, QApplication::UnicodeUTF8));
   QMdiSubWindow* subwindow = ui.mdiArea->addSubWindow(m_chart);
   subwindow->showMaximized();
	
	//connect( ui.actionBoth,         SIGNAL(triggered()), this, SLOT(SetBoth()) );
	//connect( ui.actionTime,         SIGNAL(triggered()), this, SLOT(SetTime()) );
	//connect( ui.actionManufacturer, SIGNAL(triggered()), this, SLOT(SetManufacturer()) );
	//connect( ui.actionNone,         SIGNAL(triggered()), this, SLOT(SetNone()) );
	// ---------------------------------------------------------------------------
}

void Visualization::CsvFileStatus(bool bSuccess, QString sFilename)
{
   int tmpBrk = 1;
}

void Visualization::SetTime()
{
	if( m_chart )
	{
		m_chart->SetChartView(Time);
	}
}

void Visualization::SetManufacturer()
{
	if( m_chart )
	{
		m_chart->SetChartView(Manufacturer);
	}
}

void Visualization::SetBoth()
{
	if( m_chart )
	{
		m_chart->SetChartView(Both);
	}
}

void Visualization::SetNone()
{
	if( m_chart )
	{
		m_chart->SetChartView(None);
	}
}

void Visualization::OnViewTable()
{
	TableEditor editor(sConnectionName, sDataSetName);
	editor.show();
	editor.exec();
}

void Visualization::createVisualizationUI()
{
    // Create a new mapwidget with the data we need for it and show it
}
