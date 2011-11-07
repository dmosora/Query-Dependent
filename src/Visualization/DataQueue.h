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
#ifndef _DATAQUEUE_H_
#define _DATAQUEUE_H_

#include <QMutex>
#include <QQueue>
#include <QList>

#include "DataTypes.h"


namespace Data
{

   //! Defines a thread safe FIFO queue storing data buffers.
   class DataQueue
   {
   public:

      DataQueue();
      ~DataQueue();
      

      //! Pushes a data buffer onto the queue.
      bool Enqueue( const DataBuffer& buffer );

      //! Pops the next buffer from the queue.
      bool Dequeue( DataBuffer& buffer );
      
      //! Pulls off all buffers from the queue.
      bool PopAll( QList<DataBuffer>& list );

   protected:
      QMutex              m_mutex; //!< Used to create a thread safe queue.
      QQueue<DataBuffer>  m_queue; //!< The Queue
   };
};

#endif // _DATAQUEUE_H_
