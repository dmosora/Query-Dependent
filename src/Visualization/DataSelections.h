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

#ifndef _DATASELECTIONS_H_
#define _DATASELECTIONS_H_


#include "DataMgmt.h"

namespace Data
{
   // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   //! Represents an n-dimensional point parametric with time.
   struct Point
   {
      QList<QVariant>  _dataVector; //!< Represents all parameters.
      long long        _time;       //!< Timestamp of the data vector
   };

   //! Represents metadata associated with a Point.
   struct Metadata
   {
      double      _min;         //!< Statistical min
      double      _max;         //!< Statistical max
      double      _range;       //!< Range between min and max
      ColumnDef*  _definition;  //!< Definition for the attribute
   };

   // Combination of Points over time with their associated metadata.
   struct Buffer
   {
      QList<Point>    _params;
      QList<Metadata> _metadata;
   };
   // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

   //! Class to track the data selections for a particular purpose.  The
   //! long-term application of this class will be toward abstracting the 
   //! SQL access from the system.
   class DataSelections
   {
   public:

      DataSelections();
      virtual ~DataSelections();

      //! Clears out the current selections.
      void ClearSelections();

      //! Adds an attribute to the selections.
      //! @param name  Name of the attribute to add to the selection.
      void SetSelection(const QString& name);

      //! Gets the list of currently selected attributes.
      //! @retval "Selections"  List of selected parameter names (computer name) 
      const QStringList& GetSelectedAttributes();


   protected:
      QStringList m_selections;  //!< List of selected parameters.
   };

};

#endif // _DATASELECTIONS_H_
