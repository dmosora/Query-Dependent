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
#ifndef _EVENTDETECTOR_H_
#define _EVENTDETECTOR_H_

#include <QObject>
#include <QVariant>

#include "DataMgmt.h"


namespace Event
{
   //! Class to enable scripting in the application
   class EventDetector
   {
   public:
      EventDetector( );
      ~EventDetector();

      //! Detects the events and populates the EventData structure provided
      //! @param      sFlightName Name of the flight to detect events.
      //! @param      dataMgmt    Data management object for accessing data.
      //! @param[out] evtData The detected events
      //! @retval true  If the detection was successful
      //! @retval false Otherwise
      bool DetectEvents( 
         const QString& sFlightName, 
         Data::DataMgmt* dataMgmt, 
         Data::EventData& evtData );

   };
};

#endif // _EVENTDETECTOR_H_
