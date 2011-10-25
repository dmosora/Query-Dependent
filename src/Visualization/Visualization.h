// Written by David Sheets

#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <QtGui/QMainWindow>
#include <QProgressBar>
#include <QDockWidget>
#include "ui_Visualization.h"

#include "CsvParser.h"
#include "MapWidget.h"

class Chart;

// ============================================================================
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
// ============================================================================


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

	void SetTime();
	void SetManufacturer();
	void SetBoth();
	void SetNone();

	void OnViewTable();

private:
	Ui::VisualizationClass ui;

        // UI elements for easier access
        QDockWidget*    _attributes;
        MapWidget*      _map;
        QMdiSubWindow*  _subwindow;
        QMdiSubWindow*  _mapsubwindow;

        CsvParser       m_csvParser;
        Chart*          m_chart;
        QProgressBar*   m_progress;

};

#endif // VISUALIZATION_H
