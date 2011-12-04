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
#ifndef _CHART_PARALLELCOORDINATES_H_
#define _CHART_PARALLELCOORDINATES_H_

#include <QWidget>

#include "DataSelections.h"

class QPixmap;

namespace Chart
{

   //! Defines a class that draws a parallel coordinates chart.
   class ParallelCoordinates : public QWidget
   {
      Q_OBJECT

   public:

      ParallelCoordinates(
         Data::DataSelections* selections,
         QWidget *parent = 0 );

      virtual ~ParallelCoordinates();


   protected:
      void paintEvent(QPaintEvent* event);

      int                   m_nHeight;         //!< Previous height the coordinates were drawn
      int                   m_nWidth;          //!< Previous width the coordinates were drawn
      int                   m_nNumAttrs;       //!< Number of attributes 
      Data::DataSelections* m_selections;      //!< User selected parameters
      Data::FlightDatabase  m_data;            //!< Buffer of data used by the chart

      QPixmap*              m_chart;  //!< Area the chart is drawn in.
   };
};

#endif // _CHART_PARALLELCOORDINATES_H_
