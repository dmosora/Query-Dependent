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
#include "DataNormalizer.h"

#include "EventDetector.h"

using namespace Data;

#define PRINT_EVENTS

namespace Event
{
   // These must be sequential and match the setup in DetectEvents.
   const int IASIdx      = 0;
   const int AltAglIdx   = 1;
   const int FlapHdlIdx  = 2;
   const int OnRunwayIdx = 3;
   const int GearIdx     = 4;

   
   // Max range for each event
   const int nMaxVFe40      = 210;//240;
   const int nMaxVLg        = 210;//214;
   const int nMaxVFe100     = 210;//178;
   const int nMaxVThrshld   = 150;
   const int nMaxAltThrshld = 200;
   const int nMaxVTouchdown = 125;
   
   // Min range for each event
   const int nMinVFe40      = 110;//140;
   const int nMinVLg        = 110;//114;
   const int nMinVFe100     = 110;//78;
   const int nMinVThrshld   = 50;
   const int nMinAltThrshld = 0;
   const int nMinVTouchdown = 25;

   // ==========================================================================
   // ==========================================================================
   EventDetector::EventDetector( )
   {
      // Max values for the events
      m_evtDef._maxValues.resize(nNumEvents);
      m_evtDef._maxValues[nVFe40]      = Normalizer::Normalize( 200, nMinVFe40,      nMaxVFe40);
      m_evtDef._maxValues[nVLg]        = Normalizer::Normalize( 181, nMinVLg,        nMaxVLg); // Audio said 160
      m_evtDef._maxValues[nVFe100]     = Normalizer::Normalize( 146, nMinVFe100,     nMaxVFe100);
      m_evtDef._maxValues[nVThrshld]   = Normalizer::Normalize( 113, nMinVThrshld,   nMaxVThrshld);
      m_evtDef._maxValues[nAltThrshld] = Normalizer::Normalize( 150, nMinAltThrshld, nMaxAltThrshld);
      m_evtDef._maxValues[nVTouchdown] = Normalizer::Normalize(  90, nMinVTouchdown, nMaxVTouchdown);

      // Min values for the events
      m_evtDef._minValues.resize(nNumEvents);
      m_evtDef._minValues[nVFe40]      = Normalizer::Normalize( 146, nMinVFe40,      nMaxVFe40);
      m_evtDef._minValues[nVLg]        = Normalizer::Normalize( 135, nMinVLg,        nMaxVLg);
      m_evtDef._minValues[nVFe100]     = Normalizer::Normalize( 110, nMinVFe100,     nMaxVFe100);
      m_evtDef._minValues[nVThrshld]   = Normalizer::Normalize( 101, nMinVThrshld,   nMaxVThrshld);
      m_evtDef._minValues[nAltThrshld] = Normalizer::Normalize(  45, nMinAltThrshld, nMaxAltThrshld);
      m_evtDef._minValues[nVTouchdown] = Normalizer::Normalize(  65, nMinVTouchdown, nMaxVTouchdown);

      // Min values for the events
      m_evtDef._labels.resize(nNumEvents);
      m_evtDef._labels[nVFe40]      = "VFe40";
      m_evtDef._labels[nVLg]        = "VLg";
      m_evtDef._labels[nVFe100]     = "VFe100";
      m_evtDef._labels[nVThrshld]   = "VThrshld";
      m_evtDef._labels[nAltThrshld] = "AltThrshld";
      m_evtDef._labels[nVTouchdown] = "VTouchdown";
   }

   EventDetector::~EventDetector()
   {
   }

   // ==========================================================================
   // ==========================================================================

   
   const Data::EventDefinition& EventDetector::GetEventDefinition() const
   {
      return m_evtDef;
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
         case nVLg:
            evt._eventName   = "VLg";
            evt._eventDesc   = "Landing Gear Extension (IAS)";
            evt._sequence    = 2;
            break;
         case nVFe100:
            evt._eventName   = "VFe100";
            evt._eventDesc   = "Flap Extension 100% (IAS)";
            evt._sequence    = 3;
            break;
         case nVThrshld:
            evt._eventName   = "VThrshld";
            evt._eventDesc   = "Runway Threshold (IAS)";
            evt._sequence    = 4;
            break;
         case nAltThrshld:
            evt._eventName   = "AltThrshld";
            evt._eventDesc   = "Runway Threshold (Alt)";
            evt._sequence    = 4;
            break;
         case nVTouchdown:
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
               evtData[nVFe40]._valueNormal = Normalizer::Normalize
                  ( p._dataVector.at(IASIdx).toDouble()
                  , nMinVFe40 //i.value()._metadata.at(IASIdx)._min
                  , nMaxVFe40 //i.value()._metadata.at(IASIdx)._max 
                  );
               
               eventTimes[0] = p._time;
            }
            
            if( !evtData[nVLg]._bFound && p._dataVector.at(GearIdx).toDouble() > 0.5 )
            {
               evtData[nVLg]._bFound      = true;
               evtData[nVLg]._time        = p._time;
               evtData[nVLg]._value       = p._dataVector.at(IASIdx).toDouble();
               evtData[nVLg]._valueNormal = Normalizer::Normalize
                  ( p._dataVector.at(IASIdx).toDouble()
                  , nMinVLg //i.value()._metadata.at(IASIdx)._min
                  , nMaxVLg //i.value()._metadata.at(IASIdx)._max
                  );
               
               eventTimes[1] = p._time;
            }
            
            if( !evtData[nVFe100]._bFound && p._dataVector.at(FlapHdlIdx).toDouble() == 1 )
            {
               evtData[nVFe100]._bFound      = true;
               evtData[nVFe100]._time        = p._time;
               evtData[nVFe100]._value       = p._dataVector.at(IASIdx).toDouble();
               evtData[nVFe100]._valueNormal = Normalizer::Normalize
                  ( p._dataVector.at(IASIdx).toDouble()
                  , nMinVFe100 //i.value()._metadata.at(IASIdx)._min
                  , nMaxVFe100 //i.value()._metadata.at(IASIdx)._max
                  );
               
               eventTimes[2] = p._time;
            }
            
            if( !evtData[nVThrshld]._bFound && p._dataVector.at(OnRunwayIdx).toDouble() > 0.5 )
            {
               evtData[nVThrshld]._bFound      = true;
               evtData[nVThrshld]._time        = p._time;
               evtData[nVThrshld]._value       = p._dataVector.at(IASIdx).toDouble();
               evtData[nVThrshld]._valueNormal = Normalizer::Normalize
                  ( p._dataVector.at(IASIdx).toDouble()
                  , nMinVThrshld //i.value()._metadata.at(IASIdx)._min
                  , nMaxVThrshld //i.value()._metadata.at(IASIdx)._max
                  );
               
               evtData[nAltThrshld]._bFound      = true;
               evtData[nAltThrshld]._time        = p._time+1; //!@todo Remove the plus 1
               evtData[nAltThrshld]._value       = p._dataVector.at(AltAglIdx).toDouble();
               evtData[nAltThrshld]._valueNormal = Normalizer::Normalize
                  ( p._dataVector.at(AltAglIdx).toDouble()
                  , nMinAltThrshld //i.value()._metadata.at(AltAglIdx)._min
                  , nMaxAltThrshld //i.value()._metadata.at(AltAglIdx)._max
                  );
               
               eventTimes[3] = p._time;
            }
            
            if( !evtData[nVTouchdown]._bFound && p._dataVector.at(AltAglIdx).toDouble() < 1.0 )
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
                  evtData[nVTouchdown]._bFound      = true;
                  evtData[nVTouchdown]._time        = nLandingTime;
                  evtData[nVTouchdown]._value       = fLandingIAS;
                  evtData[nVTouchdown]._valueNormal = Normalizer::Normalize
                     ( fLandingIAS
                     , nMinVTouchdown //i.value()._metadata.at(IASIdx)._min
                     , nMaxVTouchdown //i.value()._metadata.at(IASIdx)._max
                     );
               
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
