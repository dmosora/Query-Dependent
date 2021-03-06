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

#include <climits>

#include "DataTypes.h"


namespace Data
{
   // ==========================================================================
   // ==========================================================================
   ColumnDef::ColumnDef()
      : eParamType(ParamType_String)
      , sParamName("")
      , sParamNameComp("")
      , bGood(true)
   {
   }


   // ==========================================================================
   // ==========================================================================
   DataBuffer::DataBuffer()
      : bLastBuffer(false)
   {
   }

  
   // ==========================================================================
   // ==========================================================================
   EventValue::EventValue()
      : _time(-1)
      , _sequence(-1)
      , _value(-1)
      , _valueNormal(-1)
      , _bFound(false)
   {
   }
   
   // ==========================================================================
   // ==========================================================================
   LoadedFlightMetaInfo::LoadedFlightMetaInfo()
      : _uGlobalMinTime(UINT_MAX)
      , _uGlobalMaxTime(0)
   {
   }
};
