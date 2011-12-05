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

using namespace Data;

//#define PRINT_EVENTS

namespace Event
{
   // These must be sequential and match the setup in DetectEvents.
   const int IASIdx      = 0;
   const int AltAglIdx   = 1;
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

   double Normalize( double value, double min, double max )
   {
      if( min == 0 && max == 0 || min > max )
      {
         return 0;
      }

      return (value - min) / (max - min);
   }
 
   bool EventDetector::DetectEvents( 
      const QString& sFlightName, 
      Data::DataMgmt* dataMgmt, 
      Data::EventData& evtData )
   {
      Data::DataSelections dataSel;
      dataSel.SetDataMgmt(dataMgmt);
      dataSel.SetSelection( sFlightName, "Vel_Indicated_kts");
      dataSel.SetSelection( sFlightName, "Altitude_FtAgl");
      dataSel.SetSelection( sFlightName, "Flaps_Handle");
      dataSel.SetSelection( sFlightName, "RunwayThreshold");
      dataSel.SetSelection( sFlightName, "Gear");

      Data::FlightDatabase data;
      dataSel.GetDataAttributes(data);

      Data::EventValue evt;
      
      bool bPotentialLanding = false;
      int          nLandingTime;
      double       fLandingIAS;

      for( int i = 0; i < nNumEvents; ++i )
      {
         switch( i )
         {
         case nVFe40:
            evt._eventName   = "VFe40";
            evt._eventDesc   = "Flap Extension 40% (IAS)";
            evt._sequence    = 1;
            break;
         case VLg:
            evt._eventName   = "VLg";
            evt._eventDesc   = "Landing Gear Extension (IAS)";
            evt._sequence    = 2;
            break;
         case VFe100:
            evt._eventName   = "VFe100";
            evt._eventDesc   = "Flap Extension 100% (IAS)";
            evt._sequence    = 3;
            break;
         case VThrshld:
            evt._eventName   = "VThrshld";
            evt._eventDesc   = "Runway Threshold (IAS)";
            evt._sequence    = 4;
            break;
         case AltThrshld:
            evt._eventName   = "AltThrshld";
            evt._eventDesc   = "Runway Threshold (Alt)";
            evt._sequence    = 4;
            break;
         case VTouchdown:
            evt._eventName   = "VTouchdown";
            evt._eventDesc   = "Landing (IAS)";
            evt._sequence    = 4; // landing is a zero reference.
            break;
         default:
            evt._eventName   = "Unknown";
            evt._eventDesc   = "This is not a real event, something went wrong.";
         }
         
         evtData.push_back(evt);
      }
      
      QMap<int, int> eventTimes;
      Data::FlightDatabase::iterator i = data.begin();
      for( ; i != data.end(); ++i )
      {
         for( int j = 0; j < i.value()._params.size(); ++j )
         {
            const Data::Point& p = i.value()._params.at(j);
         
            if( !evtData[nVFe40]._bFound && p._dataVector.at(FlapHdlIdx).toDouble() > 0.5 )
            {
               evtData[nVFe40]._bFound      = true;
               evtData[nVFe40]._time        = p._time;
               evtData[nVFe40]._value       = p._dataVector.at(IASIdx).toDouble();
               evtData[nVFe40]._valueNormal = Normalize
                  ( p._dataVector.at(IASIdx).toDouble()
                  , i.value()._metadata.at(IASIdx)._min
                  , i.value()._metadata.at(IASIdx)._max );
               
               eventTimes[0] = p._time;
            }
            
            if( !evtData[VLg]._bFound && p._dataVector.at(GearIdx).toDouble() > 0.5 )
            {
               evtData[VLg]._bFound      = true;
               evtData[VLg]._time        = p._time;
               evtData[VLg]._value       = p._dataVector.at(IASIdx).toDouble();
               evtData[VLg]._valueNormal = Normalize
                  ( p._dataVector.at(IASIdx).toDouble()
                  , i.value()._metadata.at(IASIdx)._min
                  , i.value()._metadata.at(IASIdx)._max );
               
               eventTimes[1] = p._time;
            }
            
            if( !evtData[VFe100]._bFound && p._dataVector.at(FlapHdlIdx).toDouble() == 1 )
            {
               evtData[VFe100]._bFound      = true;
               evtData[VFe100]._time        = p._time;
               evtData[VFe100]._value       = p._dataVector.at(IASIdx).toDouble();
               evtData[VFe100]._valueNormal = Normalize
                  ( p._dataVector.at(IASIdx).toDouble()
                  , i.value()._metadata.at(IASIdx)._min
                  , i.value()._metadata.at(IASIdx)._max );
               
               eventTimes[2] = p._time;
            }
            
            if( !evtData[VThrshld]._bFound && p._dataVector.at(OnRunwayIdx).toDouble() > 0.5 )
            {
               evtData[VThrshld]._bFound      = true;
               evtData[VThrshld]._time        = p._time;
               evtData[VThrshld]._value       = p._dataVector.at(IASIdx).toDouble();
               evtData[VThrshld]._valueNormal = Normalize
                  ( p._dataVector.at(IASIdx).toDouble()
                  , i.value()._metadata.at(IASIdx)._min
                  , i.value()._metadata.at(IASIdx)._max );
               
               evtData[AltThrshld]._bFound      = true;
               evtData[AltThrshld]._time        = p._time+1; //!@todo Remove the plus 1
               evtData[AltThrshld]._value       = p._dataVector.at(AltAglIdx).toDouble();
               evtData[AltThrshld]._valueNormal = Normalize
                  ( p._dataVector.at(AltAglIdx).toDouble()
                  , i.value()._metadata.at(AltAglIdx)._min
                  , i.value()._metadata.at(AltAglIdx)._max );
               
               eventTimes[3] = p._time;
            }
            
            if( !evtData[VTouchdown]._bFound && p._dataVector.at(AltAglIdx).toDouble() < 1.0 )
            {
               if( !bPotentialLanding )
               {
                  // This is the beginning of a potential landing event.  
                  // Capture the time to see if the condition hold long 
                  // enough to count as the event.
                  nLandingTime = p._time;
                  fLandingIAS  = p._dataVector.at(IASIdx).toDouble();
                  bPotentialLanding = true;
               }
               else if( static_cast<unsigned int>(nLandingTime+150000) > p._time )
               {
                  // Landing condition must hold for 15 seconds to be true.
                  evtData[VTouchdown]._bFound      = true;
                  evtData[VTouchdown]._time        = nLandingTime;
                  evtData[VTouchdown]._value       = fLandingIAS;
                  evtData[VTouchdown]._valueNormal = Normalize
                     ( fLandingIAS
                     , i.value()._metadata.at(IASIdx)._min
                     , i.value()._metadata.at(IASIdx)._max );
               
                  eventTimes[4] = nLandingTime;
               }
            }
            else
            {
               bPotentialLanding = false;
            }
         }
      }

#ifdef PRINT_EVENTS
      QMap<int, QString> events;
      for( int i = 0; i < evtData.size(); ++i )
      {
         QString message = "   ";
         message += evtData.at(i)._eventName;
         message += "; time=";
         message += QVariant(evtData.at(i)._time).toString();
         message += "; deltaT=";
         message += QVariant(eventTimes[evtData.at(i)._sequence]-evtData.at(i)._time).toString();
         message += "; value=";
         message += QVariant(evtData.at(i)._value).toString();
         events[evtData.at(i)._time] = message;
      }

      std::cout << qPrintable(sFlightName) << std::endl;
      QMapIterator<int, QString> iMap(events);
      while( iMap.hasNext() )
      {
         std::cout << qPrintable(iMap.next().value()) << std::endl;
      }
#endif
      
      return true;
   }

};
