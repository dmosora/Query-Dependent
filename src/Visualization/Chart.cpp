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
#include <climits>

#include <QPainter>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include "Chart.h"


Chart::Chart(const QString& sConnectionName, const QString& sDataName, QWidget *parent)
	: QWidget(parent)
	, m_sConnectionName(sConnectionName)
	, m_sDataName(sDataName)
	, m_eView(Both)
{
}

Chart::~Chart()
{

}

struct DbData
{
	double Origin;
	double Manufacturer;
	double HP;
	double Disp;
	double Cylinders;
	double Accel;
	double Weight;
	double Year;
	double MPG;
};
const int NumAttributes = 8;
Q_DECLARE_METATYPE(DbData);
QList<DbData> glyphs;

struct GlyphStat
{
	double min;
	double max;
	double range;
};
Q_DECLARE_METATYPE(GlyphStat);
QList<GlyphStat> glyphStats;



void Chart::paintEvent( QPaintEvent* evtPaint )
{
	// If we haven't already, calculate and cache the data for drawing the 
	// visualization.
	if( glyphs.empty() )
	{
		GlyphStat stat;
		stat.max = INT_MIN;
		stat.min = INT_MAX;
		glyphStats.push_back(stat);
		glyphStats.push_back(stat);
		glyphStats.push_back(stat);
		glyphStats.push_back(stat);
		glyphStats.push_back(stat);
		glyphStats.push_back(stat);
		glyphStats.push_back(stat);
		glyphStats.push_back(stat);
		glyphStats.push_back(stat);
		QSqlDatabase db = QSqlDatabase::database( m_sConnectionName );
		QSqlQuery q(db);
		QString sQuery = "SELECT * from " + m_sDataName;
		if( !q.exec(sQuery) )
		{
			QSqlError err = q.lastError();
			std::cerr << "Error querying table " << qPrintable(m_sDataName) << std::endl;
			std::cerr << "   Error message: " << qPrintable( err.text() ) << std::endl;
		}
	
		// Extract the data from the database.  In the future, this 
		// information should come through another means that abstracts 
		// the database in the system.
		DbData data;
      for( int i = 0; i < 10 && q.size() > (i+9) ; ++i )
      {
         q.next();
      }
		while( q.next() )
		{
			QSqlRecord rec = q.record();
			double division = 360.0 / rec.count();
			for( int i = 0; i < rec.count() && i < glyphStats.size(); ++i )
			{
				QSqlField field = rec.field(i);
				
				stat = glyphStats.at(i);
				if( field.value().toInt() < stat.min )
				{
					stat.min = field.value().toInt();
				}
				if( field.value().toInt() > stat.max )
				{
					stat.max = field.value().toInt();
				}
				glyphStats[i] = stat;

				switch( i )
				{
				case 0:
					data.Origin = field.value().toInt();
					break;
				case 1:
					data.Manufacturer = field.value().toInt();
					break;
				case 2:
					data.HP = field.value().toInt();
					break;
				case 3:
					data.Disp = field.value().toInt();
					break;
				case 4:
					data.Cylinders = field.value().toInt();
					break;
				case 5:
					data.Accel = field.value().toInt();
					break;
				case 6:
					data.Weight = field.value().toInt();
					break;
				case 7:
					data.MPG = field.value().toInt();
					break;
				case 8:
					data.Year = field.value().toInt();
					break;
				}

			}
			glyphs.push_back(data);
		}

		// Complete the statistics calculations,
		for( int i = 0; i < glyphStats.size(); ++i )
		{
			glyphStats[i].range = glyphStats[i].max - glyphStats[i].min;
		}

		// Normalize the data so that it is in the range 0-1.
		for( int i = 0; i < glyphs.size(); ++i )
		{
			DbData data = glyphs.at(i);
			data.Origin       = (data.Origin       - glyphStats.at(0).min); // This only gets normalized to zero for a later calculation.
			data.Manufacturer = (data.Manufacturer - glyphStats.at(1).min); // This only gets normalized to zero for a later calculation.
			data.HP           = (data.HP           - glyphStats.at(2).min) / glyphStats.at(2).range;
			data.Disp         = (data.Disp         - glyphStats.at(3).min) / glyphStats.at(3).range;
			data.Cylinders    = (data.Cylinders    - glyphStats.at(4).min) / glyphStats.at(4).range;
			data.Accel        = (data.Accel        - glyphStats.at(5).min) / glyphStats.at(5).range;
			data.Weight       = (data.Weight       - glyphStats.at(6).min) / glyphStats.at(6).range;
			data.MPG          = (data.MPG          - glyphStats.at(7).min) / glyphStats.at(7).range;
			data.Year         = (data.Year         - glyphStats.at(8).min); // This only gets normalized to zero for a later calculation.

			glyphs[i] = data;
		}
	}


	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	QPainter painter(this);

#  ifdef NDEBUG
	//painter.setRenderHint(QPainter::Antialiasing);
#  endif

	int w = painter.viewport().width();
	int h = painter.viewport().height();
	this->setStyleSheet("QWidget { background-color: blue; }");
	QPoint background[4] = { QPoint(0,0), QPoint(0,h), QPoint(w,h), QPoint(w,0) };

	QPen thickPen;
	thickPen.setColor(Qt::black);
	thickPen.setWidth(2);
	QPen thinPen;
	thinPen.setColor(Qt::gray);

	QBrush brush(QColor(20,128,230));
	QBrush oldBrush = painter.brush();
	painter.setPen(thickPen);
	painter.setBrush(brush);
	painter.drawPolygon( background, 4 );
	painter.setBrush(oldBrush);


	// Draw the lines for parallel coordinates.
	int nLineSpacing = w/65;
	int nLineLength  = h/31;

	const int nNumPoints = 6;
	QPoint points[nNumPoints];
	painter.setPen(Qt::green);
	int origin = -1;
	int manufacturer = -1;
	
	int xoffset = 0;
	int yoffset = 0;
	int xIncrements = 0;
	int yIncrements = 0;
	for( int g =  0; g < glyphs.size(); ++g )
	{
		DbData data = glyphs.at(g);
		
		switch( m_eView )
		{
		case Time:
			nLineSpacing = w/5;
			nLineLength  = h/12;
			xoffset = 0;
			yoffset = (data.Year+1)*nLineLength;
			break;
		case Manufacturer:
			nLineSpacing = w/5;
			nLineLength  = h/31;
			xoffset = 0;
			yoffset = (data.Manufacturer+1)*nLineLength;
			break;
		case Both:
			nLineSpacing = w/65;
			nLineLength  = h/31;
			xoffset = data.Year*nLineSpacing*5;
			yoffset = (data.Manufacturer+1)*nLineLength;
			break;
		default:
			nLineSpacing = w/5;
			nLineLength  = h;
			xoffset = 0;
			yoffset = nLineLength;
		}

		int pt = 0;
		int x = xoffset;
		int y = 0;
		
		if( manufacturer != data.Manufacturer )
		{	
			if( origin != data.Origin )
			{
				origin = data.Origin;
				painter.setPen(thickPen);
			}
			else
			{
				painter.setPen(thinPen);
			}
			manufacturer = data.Manufacturer;
			painter.drawLine( 0, yoffset-nLineLength, w, yoffset-nLineLength);
			painter.setPen(Qt::green);
		}


		y = data.Accel * nLineLength;
		points[pt].setX(x);
		points[pt].setY(yoffset-y);
		pt++;
		x += nLineSpacing;
		
		y = data.Cylinders * nLineLength;
		points[pt].setX(x);
		points[pt].setY(yoffset-y);
		pt++;
		x += nLineSpacing;
		
		y = data.Disp * nLineLength;
		points[pt].setX(x);
		points[pt].setY(yoffset-y);
		pt++;
		x += nLineSpacing;
		
		y = data.HP * nLineLength;
		points[pt].setX(x);
		points[pt].setY(yoffset-y);
		pt++;
		x += nLineSpacing;
		
		y = data.MPG * nLineLength;
		points[pt].setX(x);
		points[pt].setY(yoffset-y);
		pt++;
		x += nLineSpacing;
		
		y = data.Weight * nLineLength;
		points[pt].setX(x);
		points[pt].setY(yoffset-y);
		pt++;
		x += nLineSpacing;
		
		painter.drawPolyline(points, nNumPoints);
	}

	for( int n = 0; n <= 12; ++n )
	{
		painter.setPen(thickPen);
		for( int m = 0; m < 6; ++m )
		{
			int x = nLineSpacing*((n*5)+m);
			painter.drawLine( x, 0, x, h);
			painter.setPen(thinPen);
		}
	}
}

void Chart::SetChartView(ViewType view)
{
	m_eView = view;
	update();
}


QSize Chart::sizeHint() const
{
   return QSize(100,100);
}