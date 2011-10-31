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
#include <QStaticText>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include <QPainter>

#include "Chart_ParallelCoordinates.h"

namespace Chart
{
   ParallelCoordinates::ParallelCoordinates(
      const QString& sConnectionName,
      const QString& sDataName,
      DataSelections* selections,
      QWidget *parent)
      : QWidget(parent)
      , m_sConnectionName(sConnectionName)
      , m_sDataName(sDataName)
      , m_selections(selections)
   {
      // We must have selections to work.
      Q_ASSERT(m_selections);

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
         Stats stat;
         stat.max = INT_MIN;
         stat.min = INT_MAX;
         QList<double> attr;

         for( int i = 0; i < nAttr; ++i )
         {
            m_attrStats.push_back(stat);
            m_chartData.push_back(attr);

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
            QSqlRecord rec = q.record();
            for( int i = 0; i < rec.count(); ++i )
            {
               QSqlField field = rec.field(i);
               
               // Extract the data value.
               double value = field.value().toDouble(&success);
               if( success )
               {
                  // Add the value to the data buffer.
                  m_chartData[i].push_back(value);

                  // Update the running statistics.
                  stat = m_attrStats.at(i);
                  if( value < stat.min )
                  {
                     stat.min = value;
                  }
                  if( value > stat.max )
                  {
                     stat.max = value;
                  }
                  m_attrStats[i] = stat;
               }
               else
               {
                  std::cerr << "Error getting data for chart. idx=" << i << std::endl;
               }
            }
         }

         // Number of statistics must match the number of attributes being 
         // displayed.
         //! @todo Combine the two lists into one data structure so that
         //!       there's less odd management such as this.
         Q_ASSERT( m_attrStats.size() == m_chartData.size() );

         // Normalize the data so that it is in the range 0-1.
         for( int i = 0; i < m_chartData.size(); ++i )
         {
            // First, calculate the range from the min and max.
            m_attrStats[i].range = m_attrStats[i].max - m_attrStats[i].min;

            // Then, update all the data by compressing it into the range 0-1.
            //! @todo I don't believe this will handle negative values.
            const Stats &stat = m_attrStats.at(i);
            for( int j = 0; j < m_chartData[i].size(); ++j )
            {
               m_chartData[i][j] = (m_chartData[i][j] - stat.min) / stat.range;
            }
         }
      }
   }

   ParallelCoordinates::~ParallelCoordinates()
   {

   }

   void ParallelCoordinates::paintEvent( QPaintEvent* evtPaint )
   {
      // ------------------------------------------------------------------------
      // ------------------------------------------------------------------------
      QPainter painter(this);

#  ifdef NDEBUG
      //painter.setRenderHint(QPainter::Antialiasing);
#  endif

      if( m_chartData.size() > 0 )
      {
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

      
         int nAttr = m_chartData.size();
         QPoint *points = new QPoint[nAttr];
         painter.setPen(Qt::green);


         // Calculate the chart parameters.
         int nLineSpacing = w/(nAttr-1);
         int nLineLength  = h;
         int xoffset = 0;
         int yoffset = nLineLength;
         int xIncrements = 0;
         int yIncrements = 0;
         for( int g =  0; g < m_chartData[0].size(); ++g )
         {
            int pt = 0;
            int x = xoffset;
            int y = 0;
                  
            for( int j = 0; j < nAttr; ++j )
            {
               const qreal& data = m_chartData[j][g];

               y = data * nLineLength;
               points[pt].setX(x);
               points[pt].setY(yoffset-y);
               pt++;
               x += nLineSpacing;
            }

            painter.drawPolyline(points, nAttr);
         }

         for( int n = 0; n <= 12; ++n )
         {
            painter.setPen(thickPen);
            for( int m = 0; m < 6; ++m )
            {
               int x = nLineSpacing*((n*nAttr)+m);
               painter.drawLine( x, 0, x, h);
               painter.setPen(thinPen);
            }
         }
      }
      else
      {
         // Draw some reminder text for now on how to get the chart to work.
         QStaticText message;

         message.setText(
            "Parallel coordinates requires loading data and selecting at least two attributes.\n"
            "Make sure data is loaded, select two attributes and then re-launch the chart.");

         painter.drawStaticText( 0, 0, message );
      }
   }
};
