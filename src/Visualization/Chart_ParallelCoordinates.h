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

namespace Chart
{
   class ParallelCoordinates : public QWidget
   {
      Q_OBJECT

      public:

         ParallelCoordinates(QWidget *parent = 0);

         virtual ~ParallelCoordinates();


         void draw();


      protected:
         void paintEvent(QPaintEvent* event);
   };
};

#endif // _CHART_PARALLELCOORDINATES_H_
