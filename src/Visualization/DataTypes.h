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

#ifndef _DATATYPES_H_
#define _DATATYPES_H_

#include <QString>
#include <QVariant>
#include <QStringList>

namespace Data
{

   //! Enumeration defining the available data types.
   enum ParamType
   {
      ParamType_String,  //!< Data is a string and cannot be converted to a number
      ParamType_Numeric  //!< Data is numeric and is treated as a floating point
   };


   //! Defines a data ColumnDef that helps link the input data names with ones
   //! that can be more easily used in the application.  This represents a single
   //! dimension in the data.
   class ColumnDef
   {
   public:
      ColumnDef();

      ParamType eParamType;     //!< The type of this data dimension
      QString   sParamName;     //!< Raw name of the dimension
      QString   sParamNameComp; //!< Processed name without spaces or special chars
      bool      bGood;          //!< Flag indicating whether the parameter is in the data.
   };


   //! Type ColumnDef to create a list of the ColumnDefs for the data in the 
   //! system.
   typedef QList<ColumnDef> ColumnDefList;


   //! Represents an n-dimensional point parametric with time.
   struct Point
   {
      QList<QVariant>  _dataVector; //!< Represents all parameters.
      unsigned int     _time;       //!< 100 microsecond increments (.1 millisecond)
   };


   //! Represents metadata associated with a Point.
   struct Metadata
   {
      double       _min;    //!< Statistical min
      double       _max;    //!< Statistical max
      double       _sum;    //!< Sum of the parameter
      double       _avg;    //!< Statistical average
      //double       _var;    //!< Statistical variance
      //double       _dev;    //!< Statistical standard deviation
      double       _range;  //!< Range between min and max
      unsigned int _count;  //!< Statistical standard deviation

      ColumnDef*  _definition;  //!< Definition for the attribute
   };


   // Combination of Points over time with their associated metadata.
   struct Buffer
   {
      QList<Point>    _params;
      QList<Metadata> _metadata;
   };

   //! Type definition representing a database
   typedef QMap<QString, Buffer> FlightDatabase;

   class DataBuffer
   {
   public:
      DataBuffer();

      QStringList data;
      QString     sFlightName;
      bool        bLastBuffer;
   };

};

#endif // _DATATYPES_H_
