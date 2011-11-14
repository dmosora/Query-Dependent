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

#include <QStringList>
#include <QMap>

#include "DataTypes.h"

namespace Data
{
   class DataMgmt;
};


namespace Data
{
   //! Definition of the selections mapping a flight to its parameters.
   typedef  QMap<QString, QStringList> Selections;

   //! Class to track the data selections for a particular purpose.  The
   //! long-term application of this class will be toward abstracting the 
   //! SQL access from the system.
   class DataSelections
   {
   public:
      //! Constant indicating that the selection applies to all flights.
      static const QString WilcardFlight;

      DataSelections();
      virtual ~DataSelections();

      //! Sets the management structure that can will be used to access the
      //! data pertaining to the selections.
      void SetDataMgmt( Data::DataMgmt* dataMgmt );

      //! Clears out the current selections.
      void ClearSelections();

      //! Adds an attribute to the selections.
      //! @param sFlightName Name of the attribute to add to the selection.
      //! @param sAttrName   Name of the attribute to add to the selection.
      void SetSelection(const QString& sFlightName, const QString& sAttrName);

      //! Gets the list of currently selected attributes.
      //! @retval "Selections"  List of selected parameter names (computer name) 
      const Selections& GetSelectedAttributes();

	  //! Populates the provided data buffer with the selected attributes.
	  //! @param data Data buffer to be populated.
	  bool GetDataAttributes(Data::FlightDatabase& data);

   protected:
      DataMgmt*   m_dataMgmt;    //!< Object used to access data.
      Selections  m_selections;  //!< List of selected parameters.
   };

};

#endif // _DATASELECTIONS_H_
