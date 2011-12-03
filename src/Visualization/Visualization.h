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
#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <QtGui/QMainWindow>
#include <QDockWidget>
#include <QGraphicsView>
#include "ui_Visualization.h"

#include "DataMgmt.h"
#include "DataSelections.h"
#include "CsvParser.h"
#include "DockWidgetAttributes.h"
#include "MapWidget.h"


// Forward declarations.
namespace Chart
{
   class ParallelCoordinates;
};


class TableEditor;


//! Represents the Main Window of the Information Visualization application.
class Visualization : public QMainWindow
{
   Q_OBJECT

public:
   Visualization(QWidget *parent = 0, Qt::WFlags flags = 0);
   ~Visualization();


protected:
   // Handles the user clicking to close the application.
   void closeEvent( QCloseEvent* event );

   void createVisualizationUI();


protected slots:
   //! Slot to handle user selection of the File->Open action.
   void LoadFile();

   //! Slot to handle enhanced status for a CSV parse.
   void DatabaseStatus(QString sFlightName);

   //! Slot that handles the completion of a CSV file thread.
   void CsvFileDone();

   //! Slot that opens the database table view.
   void OnViewTable();

   //! Slot that opens a new parallel coordinates.
   void OnViewParallelCoordinates();

   //! Slot that opens a event glyph.
   void OnViewEventGlyph();

private:
   //! Loads a single flight.
   void LoadFlight( const QString& filename );

   Ui::VisualizationClass ui;  //!< Qt generated from the .ui file from Designer

   // UI elements for easier access
   MapWidget*      _map;
   QGraphicsView   _mapview;
   QMdiSubWindow*  _subwindow;
   QMdiSubWindow*  _mapsubwindow;
   QVBoxLayout*    _mapAndSlider;
   QWidget*        _mdiMapWidget;
   TimeSlider*     _toolbar;

   Parser::CsvParser     m_csvParser; //!< Class to parse CSV files
   Data::DataMgmt        m_dataMgmt;  //!< Abstraction of the data management
   Data::DataSelections  m_attrSel;   //!< User selected data.

   DockWidgetAttributes m_dockWidgetAttr; //!< The attribute dock widget.

   Chart::ParallelCoordinates *m_viewPC;    //!< Graphical depiction of the data
   TableEditor                *m_viewTable; //!< Database editing view.

   QStringList     m_listFileNames;  //!< List of files to be opened.
   unsigned int    m_nToComplete;    //!< Value to keep track of how many flights are yet to complete.
   unsigned int    m_nNextFlightNum; //!< Next number to assign for unique names.
};

#endif // VISUALIZATION_H
