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

#include "DataQueue.h"

using namespace std;


namespace Data
{
   
   // ==========================================================================
   // ==========================================================================
   DataQueue::DataQueue( )
   {
   }

   DataQueue::~DataQueue()
   {

   }
   
   // ==========================================================================
   // ==========================================================================
   bool DataQueue::Enqueue( const DataBuffer& buffer )
   {
      m_mutex.lock();
      m_queue.enqueue( buffer );
      m_mutex.unlock();

      return true;
   }

   bool DataQueue::Dequeue( DataBuffer& buffer )
   {
      bool bRetVal(false);

      m_mutex.lock();
      if( !m_queue.empty() )
      {
         buffer = m_queue.dequeue();
         bRetVal = true;
      }
      m_mutex.unlock();
      
      return bRetVal;
   }
      
   bool DataQueue::PopAll( QList<DataBuffer>& list )
   {
      m_mutex.lock();
      while( !m_queue.empty() )
      {
         list.push_back( m_queue.dequeue() );
      }
      m_mutex.unlock();

      return true;
   }

};
