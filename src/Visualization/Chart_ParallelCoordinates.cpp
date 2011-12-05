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

#include "DataNormalizer.h"

#include "Chart_ParallelCoordinates.h"

namespace Chart
{
   ParallelCoordinates::ParallelCoordinates(
      Data::DataSelections* selections,
      QWidget *parent)
      : QWidget(parent)
      , m_nHeight(0)
      , m_nWidth(0)
      , m_nNumAttrs(0)
      , m_selections(selections)
      , m_chart(0)
   {
      // We must have selections to work.
      Q_ASSERT(m_selections);


      // This is a performance optimization..
      this->setAttribute(Qt::WA_OpaquePaintEvent);


      if( m_selections )
      {

         Data::Normalizer normalize;

         // Retrieve and process the data.
         m_selections->GetDataAttributes( m_data );

         Data::FlightDatabase::iterator i;
         for( i = m_data.begin() ; i != m_data.end(); ++i )
         {
            normalize.Process( i.value() );
            m_nNumAttrs += i.value()._metadata.size();
            if( i.value()._metadata.size() < 2 )
            {
               std::cerr << "ParallelCoordinates requires at least two selected attributes for flight ";
               std::cerr << qPrintable(i.key()) << "." << std::endl;
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
      QPainter thisPainter(this);

      int w = thisPainter.viewport().width();
      int h = thisPainter.viewport().height();
      if( (abs(m_nHeight-h) > 0 || abs(m_nWidth-w) > 0) && m_data.size() > 0 )
      {
         m_nHeight = h;
         m_nWidth  = w;

         delete m_chart;
         m_chart = new QPixmap(w, h);

         QPainter painter(m_chart);

         // Draw the background.
         QPoint background[4] = { QPoint(0,0), QPoint(0,h), QPoint(w,h), QPoint(w,0) };
         QBrush brush(QColor(20,128,230));
         QBrush oldBrush = painter.brush();
         painter.setBrush(brush);
         painter.drawPolygon( background, 4 );
         painter.setBrush(oldBrush);
         painter.setPen(Qt::green);

         Data::FlightDatabase::iterator i;
         int nFlights = m_data.size();
         int nFlightNum = -1;
         int nLineLength  = h/(nFlights);
         for( i = m_data.begin() ; i != m_data.end(); ++i )
         {
            painter.setPen(Qt::green);
            ++nFlightNum;

            int nAttr = i.value()._metadata.size();
            if( nAttr > 1 )
            {
#              ifdef NDEBUG
               // This makes the drawing VERY slow!!!
               //painter.setRenderHint(QPainter::Antialiasing);
#              endif

               this->setStyleSheet("QWidget { background-color: blue; }");

               QPoint *points = new QPoint[nAttr];

               // Calculate the chart parameters.
               int nLineSpacing = w/(nAttr-1);
               int xoffset = 0;
               int yoffset = nLineLength*(nFlightNum+1);
               int xIncrements = 0;
               int yIncrements = 0;
               bool success = false;
               for( int g =  0; g < i.value()._params.size(); g+=10 )
               {
                  int pt = 0;
                  int x = xoffset;
                  int y = 0;

                  for( int j = 0; j < i.value()._params[g]._dataVector.size(); ++j )
                  {
                     const qreal& data = i.value()._params[g]._dataVector[j].toDouble(&success);

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
                  int x = xoffset+(nLineSpacing*m);
                  painter.drawLine( x, yoffset, x, yoffset-nLineLength);
               }
               painter.drawLine
                  ( xoffset, yoffset
                  , xoffset+(nLineSpacing*nAttr), yoffset);
               painter.drawLine
                  ( xoffset, yoffset+nLineLength
                  , xoffset+(nLineSpacing*nAttr), yoffset+nLineLength);

               delete points;
            }
         }
      }

      if( m_chart )
      {
         // Draw the pixmap to the widget.
         thisPainter.drawPixmap(m_chart->rect(), *m_chart);
      }
      //      else
      //      {
      //         // QStaticText isn't available until 4.7
      //#        if QT_VERSION >= 0x040700
      //            // Draw some reminder text for now on how to get the chart to work.
      //            QStaticText message;
      //
      //            message.setText(
      //               "Parallel coordinates requires loading data and selecting at least two attributes.\n"
      //               "Make sure data is loaded, select two attributes and then re-launch the chart.");
      //
      //            painter.drawStaticText( 0, 0, message );
      //#        endif
      //      }
   }
};
