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

using namespace std;


namespace Data
{
   // ============================================================================
   // ============================================================================
   ColumnDef::ColumnDef()
      : eParamType(ParamType_String)
      , sParamName("")
      , sParamNameComp("")
      , bGood(true)
   {
   }
   // ============================================================================
   // ============================================================================

   // This ends up being a performance switch.  The number is how many data rows
   // are added to a transaction before a commit is called.  A higher number may
   // improve the time it takes to add data to the database.
   const int DataMgmt::nTransactionSwitch = 100;


   // ============================================================================
   // ============================================================================
   DataMgmt::DataMgmt( )
      : m_sConnectionName( "" )
      , m_nTransactionNum(0)
   {
   }

   DataMgmt::~DataMgmt()
   {

   }


   // ============================================================================
   // ============================================================================
   bool DataMgmt::Connect( const QString& sConnectionName, const QString& sTableName )
   {	
      // Reset the cached data.
      m_columns.clear();

      // Create the connection to the database.
      m_db = QSqlDatabase::addDatabase("QSQLITE", sConnectionName);
      m_db.setDatabaseName(sConnectionName);
      if (!m_db.open())
      {
         cerr << "Failed to open the flight database for connection: " 
            << qPrintable(sConnectionName) << endl;
         cerr << "Error Message: " << qPrintable(m_db.lastError().text()) << endl;

         return false;
      }

      // Capture the connection information.
      m_sConnectionName = sConnectionName;
      m_sTableName      = sTableName;

      // Temporary return value pending implementation.
      return true;
   }

   bool DataMgmt::ProcessHeader( const QStringList& hdr, const QStringList& data )
   {
      // Verify that the input data matches between the header row and the
      // data row.
      if( hdr.size() != data.size() )
      {
         cerr << "DataMgmt::ProcessHeader: Header names and first data row mismatch." << endl;
         return false;
      }

      // Get the database from the connection name and open a transaction.
      // Until the transaction is committed none of the executed queries are 
      // actually applied to the database.
      m_db.transaction();

      // If the database contains the indicated table already the procedure is
      // to delete it and recreate it from the data in the file being parsed.
      QSqlQuery q(m_db);
      QString sQuery = "DROP TABLE IF EXISTS ";
      sQuery.append( m_sTableName );
      if( !q.exec(sQuery) )
      {
         QSqlError err = q.lastError();
         cerr << "Error creating table " << qPrintable(m_sTableName) << endl;
         cerr << "   Error message: " << qPrintable( err.text() ) << endl;
      }

      // Construct the table based on information extracted from the CSV file.
      // The table is given an auto-incrementing ID.
      //! @todo Future iterations would be better if they allowed the input file,
      //!       user, or other source provide intelligent indexing for the data.
      ColumnDef def;
      sQuery = "CREATE TABLE " + m_sTableName + "(ID INTEGER PRIMARY KEY AUTOINCREMENT,";
      bool bSuccess = true;
      for( int i = 0; i < data.size(); ++i )
      {
         QString  column(hdr.at(i));
         QVariant dim(data.at(i));

         // Replace characters in the original name that aren't compatible 
         // with database naming conventions.
         def.sParamName = column;
         def.sParamNameComp = column.replace(" ", "");
         def.sParamNameComp = column.replace(",", "");
         def.sParamNameComp = column.replace("/", "");
         def.sParamNameComp = column.replace("-", "");
         def.sParamNameComp = column.replace("*", "");

         // Make sure that there are not duplicates in the column names.  If there are
         // it's first come first serve, ignore all subsequent columns and flag them
         // as no good.
         for( int j = 0; j < m_columns.size(); ++j )
         {
            if( m_columns.at(j).sParamNameComp == def.sParamNameComp )
            {
               // This is a duplicate parameter, set the ColumnDef accordingly.
               def.bGood = false;
               cerr << "Duplicate column names in input: " << qPrintable(def.sParamName) << endl;
               break;
            }
         }

         // If this column ColumnDef is good then figure out its data type
         // add it as a column in the query.
         if( def.bGood )
         {
            sQuery.append(def.sParamNameComp);
            dim.toFloat(&bSuccess);
            if( bSuccess )
            {
               sQuery.append(" NUMERIC,");
               def.eParamType = ParamType_Numeric;
            }
            else
            {
               sQuery.append(" VARCHAR(255),");
               def.eParamType = ParamType_String;
            }
         }

         // Note that all of the parameters are tracked, even when not placed 
         // in the table.  This is because when the data string lists are 
         // provided to ProcessData the removed columns must be filtered 
         // there as well.
         m_columns.push_back(def);
      }

      // Replace the last comma separator with a parenthesis to close out the SQL
      // query.  The string logic above just blindly places a comma after every value.
      sQuery.replace(sQuery.length()-1, 1, ')');

      // Execute the query to create the data table.
      if( !q.exec(sQuery) )
      {
         cerr << "Failed to create table " << qPrintable(m_sTableName) << " for database " << qPrintable(m_sConnectionName) << endl;
         cerr << "   Query: " << qPrintable(sQuery) << endl;
         cerr << "   Error Message: " << qPrintable(q.lastError().text()) << endl;
         m_db.rollback();
         return false;
      }

      //! @todo Cache the column ColumnDef data in the database so that if
      //!       the same file is read in while a backing database is still 
      //!       available we can just recreate all the data we need from the
      //!       database and save the time parsing the file again.
      //! sQuery = "CREATE TABLE Columns(ID INTEGER PRIMARY KEY AUTOINCREMENT,
      //!                                FriendlyName VARCHAR(32), Type VARCHAR(16))";

      // Commit the transaction.  i.e. Apply the SQL statements that have been 
      // executed since the last call to db.transaction.
      m_db.commit();

      return true;
   }

   // This takes in a string list of tokens that match the same order as the 
   // header row.  Each token is processed according to a parallel ColumnDef
   // that indicates whether the column is good and what data type it is.  The
   // data is inserted into the database.
   void DataMgmt::ProcessData( const QStringList& data )
   {	
      // If the indicated number of transactions have been queued up, then call
      // to commit the data and and begin a new transaction.
      if( (m_nTransactionNum % nTransactionSwitch) == 0 )
      {
         Commit();
      }

      // If a transaction is not currently open, create one.
      if( m_nTransactionNum == 0 )
      {
         if( !m_db.transaction() )
         {
            cerr << "Error beginning transaction." << endl;
            cerr << "   Error Message: " << qPrintable(m_db.lastError().text()) << endl;
         }
      }

      // The next transaction in this queue.
      ++m_nTransactionNum;

      // Create a query class and start constructing the query string.
      QSqlQuery q(m_db);
      QString sQueryColumns = "INSERT INTO " + m_sTableName + "(";
      QString sQueryValues  = " VALUES(";

      QString value;
      bool bSuccess = true;
      for( int i = 0; i < m_columns.size(); ++i )
      {
         // If this is a column that was ignored from the data then
         // skip this in the input data.  That is, when the header was 
         // processed this column was flagged as no good.  Therefore it
         // is ignored when inserting data into the database.
         if( !m_columns.at(i).bGood )
         {
            continue;
         }

         // Build the VALUES portion of the query according to the data
         // ColumnDef, paying attention to the 
         if( m_columns.at(i).eParamType == ParamType_String )
         {
            value = "\"" + data.at(i) + "\"";
         }
         else
         {
            value = data.at(i);

            // Make sure that if the value isn't a number that results in a null.
            QVariant dim(data.at(i));
            dim.toFloat(&bSuccess);
            if( !bSuccess )
            {
               value = "";
            }
         }

         // If there is no value for this column then just skip it in the query.
         // Since the query specifies both the column name and value this works.
         if( !value.isEmpty() )
         {
            sQueryColumns.append(m_columns.at(i).sParamNameComp);
            sQueryColumns.append(",");
            sQueryValues.append(value);
            sQueryValues.append(",");
         }
      }
      // Replace the final comma with an end parenthesis instead.  When the query
      // is generated the logic just blindly places the comma after each entry.  To
      // create a proper query the trailing comma is replaced with a parenthesis.
      sQueryColumns.replace(sQueryColumns.length()-1, 1, ')');
      sQueryValues.replace (sQueryValues.length()-1, 1, ')');
      QString sQuery = sQueryColumns + sQueryValues;

      // Execute the query and check for any error messages.
      if( !q.exec(sQuery) )
      {
         cerr << "Failed to insert data into table " << qPrintable(m_sTableName)
            << " for database " << qPrintable(m_sConnectionName) << endl;
         cerr << "   Query: " << qPrintable(sQuery) << endl;
         cerr << "   Error Message: " << qPrintable(q.lastError().text()) << endl;
      }
   }

   void DataMgmt::Commit()
   {
      // If any new transactions have been queued up then commit them.
      if( m_nTransactionNum > 0 )
      {
         if( !m_db.commit() )
         {
            cerr << "Error committing data.  Transaction rolled back." << endl;
            cerr << "   Error Message: " << qPrintable(m_db.lastError().text()) << endl;
            m_db.rollback();
         }
         m_nTransactionNum = 0;
      }
   }


   // ============================================================================
   //! @todo This will likely need to take the name of the table, which will 
   //!       correspond to the flight, so that the application can support
   //!       having multiple flights loaded.  For now, just return the single 
   //!       set of column names.
   const ColumnDefList& DataMgmt::GetColumnDefinitions(const QString& sFlightName)
   {
      return m_columns;
   }

};