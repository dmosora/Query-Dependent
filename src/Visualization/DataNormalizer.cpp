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

#include "DataNormalizer.h"


namespace Data
{

   Normalizer::Normalizer( )
   {
   }

   Normalizer::~Normalizer()
   {

   }

   double Normalizer::Normalize( double value, double min, double max )
   {
      if( min == 0 && max == 0 || min > max )
      {
         return 0;
      }

      return (value - min) / (max - min);
   }


   bool Normalizer::Process( Data::Buffer& data )
   {
      bool bSuccess;

      // Normalize the data so that it is in the range 0-1.
      for( int i = 0; i < data._params.size(); ++i )
      {
         // Then, update all the data by compressing it into the range 0-1.
         //! @todo I don't believe this will handle negative values.
         for( int j = 0; j < data._params[i]._dataVector.size(); ++j )
         {
            double d = data._params[i]._dataVector[j].toDouble(&bSuccess);
            if( bSuccess )
            {
               data._params[i]._dataVector[j] = 
                  ( d - data._metadata[j]._min) / data._metadata[j]._range;
            }
            else
            {
               std::cerr << "Error processing data." << std::endl;
               data._params[i]._dataVector[j] = data._metadata[j]._min;
            }
         }
      }

      return true;
   }

};
