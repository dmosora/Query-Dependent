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

#ifndef _DATAMGMT_H_
#define _DATAMGMT_H_

#include <QStringList>

//! @todo It'll be better to create a private implementation for this so
//!       that the include dependencies aren't transferred to users.
#include <QSqlDatabase>


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

   //! Class to abstract the storage and access of the data from the rest of the 
   //! application.  This allows the application some freedom from the underlying
   //! data storage implementation.
   class DataMgmt
   {
   public:
      //! Constant indicating the number of transactions that should be queued up
      //! before a commit is called.
      static const int nTransactionSwitch;

      DataMgmt( );
      virtual ~DataMgmt();

      //! Connects the DataMgmt to a particular data source.  The string passed
      //! in just needs to be a unique string that no other DataMgmt objects are
      //! using.  Once connected, all operations will operate on this connection.
      //! Note that in some cases it's possible that the same named connection
      //! can persist between executions of the application.
      bool Connect( const QString& sConnectionName, const QString& sTableName );

      //! Initializes the columns that will be available in the data.  The
      //! method takes in a list of header names and a sample datum in order
      //! to figure out their data type.
      //! @param hdr  String list of the column names in the data.  Invalid names
      //!             will be translated where possible.
      //! @param data String list containing sample data. Type information will 
      //!             be calculated and used to store subsequent data. The data 
      //!             passed into this function is not added to the data.  Call
      //!             ProcessData to add the data.
      //! @retval true  If the header is successfully processed
      //! @retval false Otherwise
      bool ProcessHeader( const QStringList& hdr, const QStringList& data);

      //! Process the provided string list according to the data calculated in
      //! ProcessHeader().
      //! @param data  String list of data to process and commit to data storage.
      void ProcessData( const QStringList& data );

      //! Indicates that there is no more data to process for the data.  This 
      //! will commit any outstanding transactions to the underlying storage and
      //! make it available for data processing.
      void Commit();

      //! Gets the column definitions that are currently available through this
      //! DataMgmt object.
      //! @param sFlightName  The name of the flight whose columns should be returned.
      const ColumnDefList& GetColumnDefinitions(const QString& sFlightName);


   protected:
      ColumnDefList m_columns;         //!< List of the data management by this object
      QSqlDatabase  m_db;              //!< Database being used by this class (move to pimpl)
      QString       m_sConnectionName; //!< Connection name for referencing the data
      QString       m_sTableName;      //!< @deprecated: Name of the table containing the flight data
      int           m_nTransactionNum; //!< The number of transactions since the last commit
   };
};

#endif // _DATAMGMT_H_
