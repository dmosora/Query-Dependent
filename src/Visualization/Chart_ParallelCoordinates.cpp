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
#include <cmath>

#include <QPainter>

#if QT_VERSION >= 0x040700
#  include <QStaticText>
#endif

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include <QPainter>
#include <QPixmap>

#include "Chart_ParallelCoordinates.h"

namespace Chart
{
   ParallelCoordinates::ParallelCoordinates(
      const QString& sConnectionName,
      const QString& sDataName,
      Data::DataSelections* selections,
      QWidget *parent)
      : QWidget(parent)
      , m_nHeight(0)
      , m_nWidth(0)
      , m_sConnectionName(sConnectionName)
      , m_sDataName(sDataName)
      , m_selections(selections)
	  , m_chart(0)
   {
      // We must have selections to work.
      Q_ASSERT(m_selections);

	  // This is a performance optimization..
	  this->setAttribute(Qt::WA_OpaquePaintEvent);

      // Pre-process the data for drawing.
      if( m_selections )
      {
         const QStringList& attributes = m_selections->GetSelectedAttributes();
         int nAttr = attributes.size();

         if( nAttr < 2 )
         {
            std::cerr << "ParallelCoordinates requires at least two selected attributes." << std::endl;
         }

         // Initialize the statistics data to calculate normalization and 
         // construct the query to retrieve data.
         QString sQuery = "SELECT ";
         Data::Metadata meta;
         meta._max = INT_MIN;
         meta._min = INT_MAX;

         for( int i = 0; i < nAttr; ++i )
         {
            m_data._metadata.push_back(meta);

            sQuery += attributes.at(i);
            sQuery += ",";
         }
         // Replace the last comma separator to close out the SQL query.  The 
         // string logic above just blindly places a comma after every value.
         sQuery.replace(sQuery.length()-1, 1, ' ');
         // Add the from portion to select from the correct table.
         sQuery +=  " FROM " + m_sDataName;

         QSqlDatabase db = QSqlDatabase::database( m_sConnectionName );
         QSqlQuery q(db);
         if( !q.exec(sQuery) )
         {
            QSqlError err = q.lastError();
            std::cerr << "Error querying table " << qPrintable(m_sDataName) << std::endl;
            std::cerr << "   Error message: " << qPrintable( err.text() ) << std::endl;
         }

         //! @todo This computation is expensive and a better approach needs
         //!       implemented.
         // Extract the data from the database.  In the future, this 
         // information should come through another means that abstracts 
         // the database in the system.
         bool success = false;
         while( q.next() )
         {
            Data::Point point;
            QSqlRecord rec = q.record();
            for( int i = 0; i < rec.count(); ++i )
            {
               QSqlField field = rec.field(i);

               // Extract the data value.
               double value = field.value().toDouble(&success);
               if( success )
               {
                  // Add the value to the data buffer.
                  point._dataVector.push_back(field.value());

                  // Update the running statistics.
                  if( value < m_data._metadata[i]._min )
                  {
                     m_data._metadata[i]._min = value;
                  }
                  if( value > m_data._metadata[i]._max )
                  {
                     m_data._metadata[i]._max = value;
                  }
               }
               else
               {
                  std::cerr << "Error getting data for chart. idx=" << i << std::endl;
               }
            }
            // Add the value to the data buffer.
            m_data._params.push_back(point);
         }

         for( int m = 0; m < m_data._metadata.size(); ++m )
         {
            // First, calculate the range from the min and max.
            m_data._metadata[m]._range = 
               m_data._metadata[m]._max - m_data._metadata[m]._min;
         }

         // Normalize the data so that it is in the range 0-1.
         for( int i = 0; i < m_data._params.size(); ++i )
         {
            // Then, update all the data by compressing it into the range 0-1.
            //! @todo I don't believe this will handle negative values.
            for( int j = 0; j < m_data._params[i]._dataVector.size(); ++j )
            {
               double d = m_data._params[i]._dataVector[j].toDouble(&success);
               if( success )
               {
                  m_data._params[i]._dataVector[j] = 
                     ( d - m_data._metadata[j]._min) / m_data._metadata[j]._range;
               }
               else
               {
                  std::cerr << "Error processing data." << std::endl;
                  m_data._params[i]._dataVector[j] = m_data._metadata[j]._min;
               }
            }
         }

         // Number of statistics calculated must match the number of attributes.
         Q_ASSERT( m_data._params.size() > 0 && 
                   m_data._metadata.size() == m_data._params[0]._dataVector.size() );
      }
   }

   ParallelCoordinates::~ParallelCoordinates()
   {

   }

   void ParallelCoordinates::paintEvent( QPaintEvent* evtPaint )
   {
      // ------------------------------------------------------------------------
      // ------------------------------------------------------------------------
      QPainter thisPainter(this);
	  
      int w = thisPainter.viewport().width();
      int h = thisPainter.viewport().height();
      if( (abs(m_nHeight-h) > 0 || abs(m_nWidth-w) > 0) )
      {
         m_nHeight = h;
		 m_nWidth  = w;

         delete m_chart;
		 m_chart = new QPixmap(w, h);

         QPainter painter(m_chart);

         if( m_data._params.size() > 0 )
         {
#           ifdef NDEBUG
			   // This makes the drawing VERY slow!!!
               //painter.setRenderHint(QPainter::Antialiasing);
#           endif
         
            this->setStyleSheet("QWidget { background-color: blue; }");
		 
            int nAttr = m_data._metadata.size();
            QPoint *points = new QPoint[nAttr];
            painter.setPen(Qt::green);

		    // Draw the background.
            QPoint background[4] = { QPoint(0,0), QPoint(0,h), QPoint(w,h), QPoint(w,0) };
            QBrush brush(QColor(20,128,230));
            QBrush oldBrush = painter.brush();
            painter.setBrush(brush);
            painter.drawPolygon( background, 4 );
            painter.setBrush(oldBrush);


            // Calculate the chart parameters.
            int nLineSpacing = w/(nAttr-1);
            int nLineLength  = h;
            int xoffset = 0;
            int yoffset = nLineLength;
            int xIncrements = 0;
            int yIncrements = 0;
            bool success = false;
            for( int g =  0; g < m_data._params.size(); ++g )
            {
               int pt = 0;
               int x = xoffset;
               int y = 0;

               for( int j = 0; j < nAttr; ++j )
               {
                  const qreal& data = m_data._params[g]._dataVector[j].toDouble(&success);

                  y = data * nLineLength;
                  points[pt].setX(x);
                  points[pt].setY(yoffset-y);
                  pt++;
                  x += nLineSpacing;
               }

               painter.drawPolyline(points, nAttr);
            }

		    // Draw the actual parallel coordinates last so they are visible on top.
            painter.setPen(Qt::black);
            for( int m = 0; m < nAttr; ++m )
            {
               int x = nLineSpacing*m;
               painter.drawLine( x, 0, x, h);
            }
         }
         else
         {
            // QStaticText isn't available until 4.7
#           if QT_VERSION >= 0x040700
               // Draw some reminder text for now on how to get the chart to work.
               QStaticText message;

               message.setText(
                  "Parallel coordinates requires loading data and selecting at least two attributes.\n"
                  "Make sure data is loaded, select two attributes and then re-launch the chart.");

               painter.drawStaticText( 0, 0, message );
#           endif
         }
	  }

      // Draw the pixmap to the widget.
      thisPainter.drawPixmap(m_chart->rect(), *m_chart);
   }
};
