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


namespace Chart
{
   //! Simple structure for storing statistics calculated for an
   //! attribute.
   struct Stats
   {
      double min;
      double max;
      double range;
   };


   class ParallelCoordinates : public QWidget
   {
      Q_OBJECT

   public:

      ParallelCoordinates(
         const QString& sConnectionName,
         const QString& sDataName,
         DataSelections* selections,
         QWidget *parent = 0 );

      virtual ~ParallelCoordinates();


      void draw();


   protected:
      void paintEvent(QPaintEvent* event);


      QString         m_sConnectionName; //!< Name of the database connection
      QString         m_sDataName;       //!< Name of the table containing the data
      DataSelections* m_selections;      //!< User selected parameters.
      
      //! @todo Combine these into one structure.
      QList<Stats> m_attrStats;
      QList<QList<double> > m_chartData; //!< Processes chart data.
   };
};

#endif // _CHART_PARALLELCOORDINATES_H_
