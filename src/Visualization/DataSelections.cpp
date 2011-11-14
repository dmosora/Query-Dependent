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

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include "DataMgmt.h"
#include "DataSelections.h"



namespace Data
{
   // ==========================================================================
   // ==========================================================================
   const QString DataSelections::WilcardFlight = "*";


   // ==========================================================================
   // ==========================================================================
   DataSelections::DataSelections()
      : m_dataMgmt(0)
   {
   }

   DataSelections::~DataSelections()
   {

   }


   // ==========================================================================
   // ==========================================================================
   void DataSelections::SetDataMgmt( DataMgmt* dataMgmt )
   {
      m_dataMgmt = dataMgmt;
   }

   void DataSelections::ClearSelections()
   {
      m_selections.clear();
   }

   void DataSelections::SetSelection(const QString& sFlightName, const QString& sAttrName)
   {
      m_selections[sFlightName].push_back(sAttrName);
   }

   const Selections& DataSelections::GetSelectedAttributes()
   {
      return m_selections;
   }

   bool DataSelections::GetDataAttributes(Data::FlightDatabase& data)
   {
      if( !m_dataMgmt )
      {
         return false;
      }

      bool retVal = true;

      // Clear out the lists in preparation to populate them.
      data.clear();

      // Iterate over all of the selections and query each flight.
      Selections::iterator i;
      for( i = m_selections.begin() ; i != m_selections.end(); ++i )
      {
         if( i.key() == WilcardFlight )
         {
            // The wildcard results in a request to all flights.
            QStringList flights;
            m_dataMgmt->GetLoadedFlights(flights);
            QStringListIterator fIdx(flights);
            while( fIdx.hasNext() )
            {
               QString flight = fIdx.next();
               retVal &= m_dataMgmt->GetDataAttributes(flight, i.value(), data[flight]);
            }
         }
         else
         {
            retVal &= m_dataMgmt->GetDataAttributes(i.key(), i.value(), data[i.key()]);
         }
      }

      return retVal;
   }

};
