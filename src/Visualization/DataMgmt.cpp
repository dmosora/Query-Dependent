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

   // Conversion from hours to 100 microsecond increments.
   // 60 minutes per hour, 60 seconds per minute, 
   // 1,000 milliseconds per second,
   // 10 100 microsecond increments per millisecond
   const unsigned int HoursTo100MicroSeconds = 60*60*1000*10;

   // This ends up being a performance switch.  The number is how many data rows
   // are added to a transaction before a commit is called.  A higher number may
   // improve the time it takes to add data to the database.
   const int DataMgmt::nTransactionSwitch = 500;


   // ==========================================================================
   // ==========================================================================
   DataMgmt::DataMgmt( )
      : m_sConnectionName( "" )
      , m_bStop(false)
      , m_nProcessed(0)
   {
      //! @todo This should really be done by the Event Detector but what 
      //!       really needs to happen is that the event detector needs to be 
      //!       integrated into the data management a little better or the 
      //!       events removed from data management altogether.

      // Max values for the events
      m_evtDb._maxValues.resize(nNumEvents);
      m_evtDb._maxValues[nVFe40]     = 200;
      m_evtDb._maxValues[VLg]        = 181; // Audio said 160
      m_evtDb._maxValues[VFe100]     = 146;
      m_evtDb._maxValues[VThrshld]   = 108;
      m_evtDb._maxValues[AltThrshld] = 65;
      m_evtDb._maxValues[VTouchdown] = 69;

      // Min values for the events
      m_evtDb._minValues.resize(nNumEvents);
      m_evtDb._minValues[nVFe40]     = 140;
      m_evtDb._minValues[VLg]        = 140;
      m_evtDb._minValues[VFe100]     = 140;
      m_evtDb._minValues[VThrshld]   = 103;
      m_evtDb._minValues[AltThrshld] = 45;
      m_evtDb._minValues[VTouchdown] = 64;

      // Min values for the events
      m_evtDb._labels.resize(nNumEvents);
      m_evtDb._labels[nVFe40]     = "VFe40";
      m_evtDb._labels[VLg]        = "VLg";
      m_evtDb._labels[VFe100]     = "VFe100";
      m_evtDb._labels[VThrshld]   = "VThrshld";
      m_evtDb._labels[AltThrshld] = "AltThrshld";
      m_evtDb._labels[VTouchdown] = "VTouchdown";
   }

   DataMgmt::~DataMgmt()
   {
      stopProcessing();
      wait();

      m_db.close();
   }


   // ==========================================================================
   // Database logic
   // ==========================================================================
   bool DataMgmt::Connect( const QString& sConnectionName )
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

      // Temporary return value pending implementation.
      return true;
   }

   bool DataMgmt::ProcessHeader( 
      const QString& sFlightName,
      const QStringList& hdr,
      const QStringList& data )
   {
      // Verify that the input data matches between the header row and the
      // data row.
      if( hdr.size() != data.size() )
      {
         cerr << "DataMgmt::ProcessHeader: Header names and first data row mismatch." << endl;
         return false;
      }

      Data::DataBuffer buffer;
      buffer.sFlightName = sFlightName;
      
      // If the database contains the indicated table already the procedure is
      // to delete it and recreate it from the data in the file being parsed.
      QSqlQuery q(m_db);
      QString sQuery = "DROP TABLE IF EXISTS ";
      sQuery.append( sFlightName );
      buffer.data.push_back( sQuery );

      // Construct the table based on information extracted from the CSV file.
      // The table is given an auto-incrementing ID.
      //! @todo Future iterations would be better if they allowed the input file,
      //!       user, or other source provide intelligent indexing for the data.
      ColumnDef     def;
      ColumnDefList defList;
      sQuery = "CREATE TABLE " + sFlightName + "(ID INTEGER PRIMARY KEY AUTOINCREMENT,";
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
         for( int j = 0; j < defList.size(); ++j )
         {
            if( defList.at(j).sParamNameComp == def.sParamNameComp )
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
         defList.push_back(def);
      }

      // Save off the column definitions.
      m_columns[sFlightName] = defList;

      // Replace the last comma separator with a parenthesis to close out the SQL
      // query.  The string logic above just blindly places a comma after every value.
      sQuery.replace(sQuery.length()-1, 1, ')');

      // Place the query in the buffer for processing.
      buffer.data.push_back( sQuery );

      //! @todo Cache the column ColumnDef data in the database so that if
      //!       the same file is read in while a backing database is still 
      //!       available we can just recreate all the data we need from the
      //!       database and save the time parsing the file again.
      //! sQuery = "CREATE TABLE Columns(ID INTEGER PRIMARY KEY AUTOINCREMENT,
      //!                                FriendlyName VARCHAR(32), Type VARCHAR(16))";

      // Enqueue the commands onto the buffer for processing.
      m_queue.Enqueue( buffer );

      return true;
   }

   // This takes in a string list of tokens that match the same order as the 
   // header row.  Each token is processed according to a parallel ColumnDef
   // that indicates whether the column is good and what data type it is.  The
   // data is inserted into the database.
   void DataMgmt::ProcessData( 
      const QString& sFlightName,
      const QStringList& data,
      DataBuffer& buffer )
   {
      // Find the column names matching this flight.
      FlightColumnMap::const_iterator i = m_columns.find(sFlightName);
      if( i == m_columns.constEnd() )
      {
         cerr << "Process data called on a flight without a header.  Cannot process" << endl;
         return;
      }
      const ColumnDefList& defList = i.value();
      
      // Create a query class and start constructing the query string.
      QString sQueryColumns = "INSERT INTO " + sFlightName + "(";
      QString sQueryValues  = " VALUES(";

      QString value;
      bool bSuccess = true;
      for( int i = 0; i < defList.size(); ++i )
      {
         // If this is a column that was ignored from the data then
         // skip this in the input data.  That is, when the header was 
         // processed this column was flagged as no good.  Therefore it
         // is ignored when inserting data into the database.
         if( !defList.at(i).bGood )
         {
            continue;
         }

         // Build the VALUES portion of the query according to the data
         // ColumnDef, paying attention to the 
         if( defList.at(i).eParamType == ParamType_String )
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
            sQueryColumns.append(defList.at(i).sParamNameComp);
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

      buffer.data.push_back(sQuery);
      if( buffer.data.size() > nTransactionSwitch )
      {
         Commit( buffer );
      }
   }
   
   
   // ==========================================================================
   // Accessor methods
   // ==========================================================================
   const ColumnDefList& DataMgmt::GetColumnDefinitions(const QString& sFlightName) const
   {
      static Data::ColumnDefList NullColumnDef;

      FlightColumnMap::const_iterator i = m_columns.find(sFlightName);

      if( i == m_columns.constEnd() )
      {
         return NullColumnDef;
      }

      return i.value();
   }
   
   bool DataMgmt::GetLoadedFlights(QStringList& flights) const
   {      
      FlightColumnMap::const_iterator i = m_columns.begin();
      for( ; i != m_columns.end(); ++i )
      {
         flights.push_back( i.key() );
      }

      return true;
   }

   bool DataMgmt::GetDataAttributes(
      const QString& sFlight,
      const QStringList& attributes,
      Data::Buffer& data)
   {
      int nAttr = attributes.size();

      // Initialize the statistics data to calculate normalization and 
      // construct the query to retrieve data.
      QString sQuery = "SELECT Time_Hours,";
      Data::Metadata meta;
      meta._max = INT_MIN;
      meta._min = INT_MAX;
      meta._sum = 0;
      meta._count = 0;

      for( int i = 0; i < nAttr; ++i )
      {
         data._metadata.push_back(meta);

         sQuery += attributes.at(i);
         sQuery += ",";
      }
      // Replace the last comma separator to close out the SQL query.  The 
      // string logic above just blindly places a comma after every value.
      sQuery.replace(sQuery.length()-1, 1, ' ');
      // Add the from portion to select from the correct table.
      sQuery +=  " FROM " + sFlight;

      QSqlDatabase db = QSqlDatabase::database( m_sConnectionName );
      QSqlQuery q(db);
      if( !q.exec(sQuery) )
      {
         QSqlError err = q.lastError();
         std::cerr << "Error querying table " << qPrintable(sFlight) << std::endl;
         std::cerr << "   Error message: " << qPrintable( err.text() ) << std::endl;
      }

      // Extract the data from the database.
      bool bSuccess = false;
      while( q.next() )
      {
         Data::Point point;
         QSqlRecord rec = q.record();

         // The first record should always be time.
         QSqlField field = rec.field(0);
         double time = field.value().toDouble(&bSuccess);
         if( bSuccess )
         {
            //! @todo Time conversion might be better as a process into
            //!       database vs. coming out but at least it's all
            //!       abstracted behind this class.
            // Convert hours to the 100 microsecond increments.
            point._time = time * HoursTo100MicroSeconds;
         }
         else
         {
            point._time = 0;
         }

         int m = 0;
         for( int i = 1; i < rec.count(); ++i )
         {
            m = i-1;
            field = rec.field(i);

            // Extract the data value.
            double value = field.value().toDouble(&bSuccess);
            if( bSuccess )
            {
               // Add the value to the data buffer.
               point._dataVector.push_back(field.value());

               // Update the running statistics.
               data._metadata[m]._sum += value;
               ++data._metadata[m]._count;
               if( value < data._metadata[m]._min )
               {
                  data._metadata[m]._min = value;
               }
               if( value > data._metadata[m]._max )
               {
                  data._metadata[m]._max = value;
               }
            }
            else
            {
               std::cerr << "Error getting data for chart. idx=" << i << std::endl;
            }
         }
         // Add the value to the data buffer.
         data._params.push_back(point);
      }

      for( int m = 0; m < data._metadata.size(); ++m )
      {
         // First, calculate the range from the min and max.
         data._metadata[m]._range = 
            data._metadata[m]._max - data._metadata[m]._min;

         data._metadata[m]._avg = data._metadata[m]._sum	/ data._metadata[m]._count;
      }

      // Number of statistics calculated must match the number of attributes.
      Q_ASSERT( data._params.size() > 0 && 
         data._metadata.size() == data._params[0]._dataVector.size() );

      return true;
   }

   
   bool DataMgmt::SetEventData( QString sFlightName, const EventData& evtData )
   {
      //! @todo EventData should really be combined with an ability to replace
      //!       based on the event definition.  For now, it's a replace.
      m_evtDb._events[sFlightName] = evtData;

      return true;
   }

   const EventDatabase& DataMgmt::GetEventData( ) const
   {
      return m_evtDb;
   }
   
   
   // ==========================================================================
   // Threading methods
   // ==========================================================================
   void DataMgmt::Commit(DataBuffer& buffer)
   {
      // This is the producer portion of the threading.  This method is called
      // by another thread, which places the data into the queue.
      if( !buffer.data.empty() )
      {
         m_queue.Enqueue(buffer);
         buffer.data.clear();
      }
   }
  
   void DataMgmt::run()
   {
      // This is the consumer of the threaded data read.  It pulls items off 
      // the queue and performs the call to the database.
      DataBuffer buffer;
      while( !m_bStop )
      {
         if( m_queue.Dequeue(buffer) )
         {
            m_mutex.lock();
            m_db.transaction();

            QSqlQuery q(m_db);
            for( int i = 0; i < buffer.data.size(); ++i )
            {
               // Execute the query and check for any error messages.
               if( !q.exec(buffer.data.at(i)) )
               {
                  cerr << "------------------------------------------" << endl;
                  cerr << "Query failed for database " << qPrintable(m_sConnectionName) << endl;
                  cerr << "   Flight       : " << qPrintable(buffer.sFlightName) << endl;
                  cerr << "   Query        : " << qPrintable(buffer.data.at(i));
                  cerr << "   Error Message: " << qPrintable(q.lastError().text()) << endl;
                  cerr << "------------------------------------------" << endl;
               }
            }

            // Track progress.
            ++m_nProcessed;
            emit( setProgressRange(0, m_nProcessed+m_queue.Size()) );
            emit( setCurrentProgress(m_nProcessed) );

            m_db.commit();
            m_mutex.unlock();

            if( buffer.bLastBuffer )
            {
               emit( FlightComplete(buffer.sFlightName) );
            }
         }
         else
         {
            // Give some time to allow the queue to populate but don't
            // hog all the processing time.
            QThread::msleep(500);
         }
      }

      m_bStop = false; // In case the thread needs restarted.
   }

   void DataMgmt::stopProcessing()
   {
      m_mutex.lock();
      m_bStop = true;
      m_mutex.unlock();
   }

   // ==========================================================================
   // ==========================================================================

};
