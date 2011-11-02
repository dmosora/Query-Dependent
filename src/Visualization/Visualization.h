// Written by David Sheets

#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <QtGui/QMainWindow>
#include <QProgressBar>
#include <QDockWidget>
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


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// Threading test code
#include <iostream>
class MyObject : public QObject
{
   Q_OBJECT
      public slots:
         void MySlot( int increment )
         {
            std::cout << "slot called " << increment << std::endl;
         }
};

class CThread1 : public QThread
{
   Q_OBJECT
public:
   void run( void )
   {
      std::cout << "thread 1 started" << std::endl;
      int i = 0;
      while(1)
      {
         msleep( 2000 );
         emit MySignal(i++);
      }
   }
signals:
   void MySignal( int increment );
};
// End threading test code.
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//! Represents the Main Window of the Information Visualization application.
class Visualization : public QMainWindow
{
   Q_OBJECT

public:
   Visualization(QWidget *parent = 0, Qt::WFlags flags = 0);
   ~Visualization();

protected:
   void closeEvent( QCloseEvent* event );
   void createVisualizationUI();

protected slots:
   void LoadFile();
   void CsvFileStatus(bool bSuccess, QString sFilename);
   void CsvFileDone();


   //! Slot that opens the database table view.
   void OnViewTable();

   //! Slot that opens a new parallel coordinates.
   void OnViewParallelCoordinates();

private:
   Ui::VisualizationClass ui;  //!< Qt generated from the .ui file from Designer

   // UI elements for easier access
   MapWidget*      _map;
   QMdiSubWindow*  _subwindow;
   QMdiSubWindow*  _mapsubwindow;

   QProgressBar*         m_progress;  //!< Progress bar displayed in the status bar
   Parser::CsvParser     m_csvParser; //!< Class to parse CSV files
   Data::DataMgmt        m_dataMgmt;  //!< Abstraction of the data management
   Data::DataSelections  m_attrSel;   //!< User selected data.

   DockWidgetAttributes m_dockWidgetAttr; //!< The attribute dock widget.

   Chart::ParallelCoordinates *m_viewPC;    //!< Graphical depiction of the data
   TableEditor                *m_viewTable; //!< Database editing view.
};

#endif // VISUALIZATION_H
