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

#ifndef _DATAPROCESSOR_H_
#define _DATAPROCESSOR_H_


#include "DataTypes.h"

namespace Data
{
   //! Class to abstract a data manipulation routine.
   class Processor
   {
   public:

      Processor( );
      virtual ~Processor();


      //! Performs a calculation on the data according to the specific
      //! implementation.
      //! @param data        Buffer of data to which the requested parameters are added.
      //! @retval true  If the operation exceeds entirely
      //! @retval false If any portion of the operation fails.
      virtual bool Process(Data::Buffer& data) = 0;

   protected:
   };
};

#endif // _DATAPROCESSOR_H_
