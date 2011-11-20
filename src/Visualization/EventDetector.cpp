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

#include "DataSelections.h"

#include "EventDetector.h"


namespace Event
{
   // These must be sequential and match the setup in DetectEvents.
   const int IASIdx      = 0;
   const int AltIdx      = 1;
   const int FlapHdlIdx  = 2;
   const int OnRunwayIdx = 3;
   const int GearIdx     = 4;

   // ==========================================================================
   // ==========================================================================
   EventDetector::EventDetector( )
   {
   }

   EventDetector::~EventDetector()
   {
   }

   // ==========================================================================
   // ==========================================================================
   
   bool EventDetector::DetectEvents( const QString& sFlightName, Data::DataMgmt* dataMgmt, EventData& evtData )
   {
      Data::DataSelections dataSel;
      dataSel.SetDataMgmt(dataMgmt);
      dataSel.SetSelection( sFlightName, "Vel_Indicated_kts");
      dataSel.SetSelection( sFlightName, "Altitude_FtMsl");
      dataSel.SetSelection( sFlightName, "Flaps_Handle");
      dataSel.SetSelection( sFlightName, "RunwayThreshold");
      dataSel.SetSelection( sFlightName, "Gear");

      Data::FlightDatabase data;
      dataSel.GetDataAttributes(data);

      EventValue evt;

      bool bFe40Found   = false;
      bool bFe100Found  = false;
      bool bLgFound     = false;
      bool bThreshFound = false;

      Data::FlightDatabase::iterator i = data.begin();
      for( ; i != data.end(); ++i )
      {
         for( int j = 0; j < i.value()._params.size(); ++j )
         {
            const Data::Point& p = i.value()._params.at(j);
         
            if( ! bFe40Found && p._dataVector.at(FlapHdlIdx).toDouble() > 0.5 )
            {
               bFe40Found = true;

               evt._eventName = "Flap Extension 40% (IAS)     ";
               evt._time      = p._time;
               evt._value     = p._dataVector.at(IASIdx);
               evtData.push_back(evt);
            }
            
            if( ! bFe100Found && p._dataVector.at(FlapHdlIdx).toDouble() == 1 )
            {
               bFe100Found = true;

               evt._eventName = "Flap Extension 100% (IAS)    ";
               evt._time      = p._time;
               evt._value     = p._dataVector.at(IASIdx);
               evtData.push_back(evt);
            }
            
            if( ! bLgFound && p._dataVector.at(GearIdx).toDouble() > 0.5 )
            {
               bLgFound = true;

               evt._eventName = "Landing Gear Extension (IAS) ";
               evt._time      = p._time;
               evt._value     = p._dataVector.at(IASIdx);
               evtData.push_back(evt);
            }
            
            if( ! bThreshFound && p._dataVector.at(OnRunwayIdx).toDouble() > 0.5 )
            {
               bThreshFound = true;

               evt._eventName = "Runway Threshold (IAS)       ";
               evt._time      = p._time;
               evt._value     = p._dataVector.at(IASIdx);
               evtData.push_back(evt);

               evt._eventName = "Runway Threshold (Alt)       ";
               evt._time      = p._time+1; //!@todo Remove the plus 1
               evt._value     = p._dataVector.at(AltIdx);
               evtData.push_back(evt);
            }
            
            if( ! bLgFound && p._dataVector.at(GearIdx).toDouble() > 0.5 )
            {
               bLgFound = true;

               evt._eventName = "Landing Gear Extension (IAS) ";
               evt._time      = p._time;
               evt._value     = p._dataVector.at(IASIdx);
               evtData.push_back(evt);
            }
         }
      }

      QMap<unsigned int, QString> events;
      for( int i = 0; i < evtData.size(); ++i )
      {
         QString message = "   ";
         message += evtData.at(i)._eventName;
         message += "; time=";
         message += QVariant(evtData.at(i)._time).toString();
         message += "; value=";
         message += QVariant(evtData.at(i)._value).toString();
         events[evtData.at(i)._time] = message;
      }

      std::cout << qPrintable(sFlightName) << std::endl;
      QMapIterator<unsigned int, QString> iMap(events);
      while( iMap.hasNext() )
      {
         std::cout << qPrintable(iMap.next().value()) << std::endl;
      }


      return true;
   }

};
