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

#include <QMutex>
#include <QThread>

#include <QStringList>
#include <QMap>

#include <QSqlDatabase>

#include "DataTypes.h"
#include "DataQueue.h"


namespace Data
{
   //! Defines a type to store the column definitions for each flight.
   typedef QMap<QString, Data::ColumnDefList> FlightColumnMap;

   //! Class to abstract the storage and access of the data from the rest of the 
   //! application.  This allows the application some freedom from the underlying
   //! data storage implementation.
   class DataMgmt : public QThread
   {
      Q_OBJECT

   public:
      //! Constant indicating the number of transactions that should be queued up
      //! before a commit is called.
      static const int nTransactionSwitch;

      DataMgmt( );
      ~DataMgmt();

      //! Connects the DataMgmt to a particular data source.  The string passed
      //! in just needs to be a unique string that no other DataMgmt objects are
      //! using.  Once connected, all operations will operate on this connection.
      //! Note that in some cases it's possible that the same named connection
      //! can persist between executions of the application.
      //! @param sConnectionName Name of the database that this manager will use
      bool Connect( const QString& sConnectionName );

      //! Initializes the columns that will be available in the data.  The
      //! method takes in a list of header names and a sample datum in order
      //! to figure out their data type.
      //! @param sFlightName Unique identifier for the flight
      //! @param hdr  String list of the column names in the data.  Invalid names
      //!             will be translated where possible.
      //! @param data String list containing sample data. Type information will 
      //!             be calculated and used to store subsequent data. The data 
      //!             passed into this function is not added to the data.  Call
      //!             ProcessData to add the data.
      //! @retval true  If the header is successfully processed
      //! @retval false Otherwise
      bool ProcessHeader( 
         const QString& sFlightName,
         const QStringList& hdr,
         const QStringList& data );

      //! Process the provided string list according to the data calculated in
      //! ProcessHeader().
      //! @param sFlightName Unique identifier for the flight
      //! @param data  String list of data to process and commit to data storage.
      void ProcessData( 
         const QString& sFlightName,
         const QStringList& data,
         DataBuffer& buffer );

      //! Indicates that there is no more data to process for the data.  This 
      //! will commit any outstanding transactions to the underlying storage and
      //! make it available for data processing.
      void Commit(DataBuffer& buffer);


      //! Gets the column definitions that are currently available through this
      //! DataMgmt object.
      //! @param sFlightName  The name of the flight whose columns should be returned.
      const ColumnDefList& GetColumnDefinitions(const QString& sFlightName) const;

      //! Provides a list of the currently loaded flights.
      //! @param flights List that each flight name will be added.
      //! @retval true  If the operation succeeds
      //! @retval false Otherwise
      bool GetLoadedFlights(QStringList& flights) const;


      //! Retrieves the data pertaining to the attributes selected by this object.
      //! @param sFlight     The unique identifier for the flight.
      //! @param attributes  List of attributes to add
      //! @param data        Buffer of data to which the requested parameters are added.
      //! @retval true  If the operation exceeds entirely
      //! @retval false If any portion of the operation fails.
      bool GetDataAttributes(
          const QString& sFlight,
          const QStringList& attributes,
          Data::Buffer& data );

      //! Sets event definition
      void SetEventDefinition( const EventDefinition& evtDef );

      //! Adds the event data to the data management.
      //! @param sFlightName  Name of the flight to which events will be added
      //! @param evtData      Event data to add
      bool SetEventData( QString sFlightName, const EventData& evtData );
      
      //! Provides access to the event database.
      const EventDatabase& GetEventData( ) const;

      //! Gets the metadata structure for all loaded flights.
      const LoadedFlightMetaInfo& GetLoadedFlightMetaInfo() const;
      
   public slots:
      //! Slot to handle an interrupt signal.  This will stop the data processing.
      void stopProcessing();

   signals:
      //! Signal sent when the last queue entry for a flight has been processed.
      //! @param sFileName Provides the file that was parsed.
      void FlightComplete(QString sFileName);

      //! Emits the range of progress increments that will be reported during
      //! parsing of the file by setCurrentProgress() signal.
      //! @param min  First number reported as 0% progress.
      //! @param max  Last number reports as 100% progress.
      void setProgressRange(int min, int max);

      //! Sets the current progress in parsing the file.
      //! @param value  A number indicating a range of completeness from the min
      //!               and max values emitted by setProgressRange().
      void setCurrentProgress(int value);

   protected:
      //! The threaded functionality.
      void run();


      mutable QMutex  m_mutex;           //!< Mutex for thread safety
      FlightColumnMap m_columns;         //!< List of the data management by this object
      QSqlDatabase    m_db;              //!< Database being used by this class (move to pimpl)
      QString         m_sConnectionName; //!< Connection name for referencing the data
      Data::DataQueue m_queue;           //!< Queue containing parsed flight data
      bool            m_bStop;           //!< Flag indicating that parsing should stop
      int             m_nProcessed;      //!< Running count of the buffers processed.
      QStringList     m_loadedFlights;   //!< List of flights that have completed loading

      EventDatabase   m_evtDb;           //!< Event data mapped to each flight.

      LoadedFlightMetaInfo m_flightMeta; //!< Meta data on the flights that are loaded.
   };
};

#endif // _DATAMGMT_H_
