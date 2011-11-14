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

#include <QPixmap>
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QVariant>

#include "DataMgmt.h"
#include "CsvParser.h"

using namespace std;

namespace Parser
{
   CsvParser::CsvParser(QObject* parent)
      : m_dataMgmt(0)
      , m_cDelim(',')
      , m_bStop(false)
      , m_bHasHeader(true)
   {
   }

   CsvParser::~CsvParser()
   {
      stopParse();

      wait();
   }

   void CsvParser::SetParseInformation( 
      const QString& sFilename,
      Data::DataMgmt* dataMgmt,
      const QString& sFlightName,
      const QString& sConnection )
   {
      // When a new file is received there is a new definition which means a new
      // database configuration.  Clear the definition list to ensure there aren't
      // any stagnant information.
      m_mutex.lock();
      m_sFilename = sFilename;
      m_dataMgmt = dataMgmt;
      m_sFlightName = sFlightName;
      m_buffer.sFlightName = sFlightName;
      m_mutex.unlock();
   }

   const QString& CsvParser::GetFlightName() const
   {
      return m_sFlightName;
   }

   const int nProgressIncrements = 100;
   void CsvParser::run()
   {
      if( !m_dataMgmt )
      {
         cerr << qPrintable(tr("CsvParser: No DataMgmt provided. ")) 
            << qPrintable(m_sFilename) << qPrintable(tr(" cannot be parsed")) << endl;
         return;
      }

      QFile file( m_sFilename );
      if( file.open( QIODevice::ReadOnly ) )
      {
         QStringList tokens;

         QTextStream stream(&file);
         QString line = stream.readLine();

         // Setup the progress indication.
         qint64 llFileSize = file.size();
         qint64 llFilePos = stream.pos();
         qint64 llFileInc = llFileSize/nProgressIncrements;
         int    nLastProg = 0;
         emit( setProgressRange(0, nProgressIncrements) );
         emit( setCurrentProgress(0) );

         // Process header information.
         //! @todo The algorithm currently requires header information.  This
         //!       isn't specifically required because we could just make up 
         //!       names for the parameters or add the ability to hand in the
         //!       definition data through the API.
         if( m_bHasHeader && !line.isNull() )
         {
            QStringList hdrTokens;
            QString header = line;
            if( ExtractTokens(header, hdrTokens) )
            {
               line = stream.readLine();
               if( !line.isNull() && ExtractTokens( line, tokens ) )
               {
                  if( !m_dataMgmt->ProcessHeader( m_sFlightName, hdrTokens, tokens ) )
                  {
                     cerr << qPrintable(tr("Error extracting header information.")) << endl;
                     emit( fileDoneStatus(false, m_sFilename) );
                     return;
                  }
               }
            }
         }

         // Read in the rest of the file.
         while( !line.isNull() )
         {
            // Extract tokens from the current line.
            ExtractTokens( line, tokens );

            // Update progress.
            if( (llFilePos - (llFileInc*nLastProg)) > llFileInc )
            {
               // Calculate a progress increment
               ++nLastProg;
               emit( setCurrentProgress(nLastProg) );
            }

            // Try to get the next line in order to determine if this is the 
            // last line in the file
            line = stream.readLine();
            llFilePos += line.length();
            m_buffer.bLastBuffer = line.isNull();

            // Process the current data
            m_dataMgmt->ProcessData( m_sFlightName, tokens, m_buffer );
         }
      }

      // Complete the data storage process.
      if( m_buffer.data.size() )
      {
         m_buffer.bLastBuffer = true;
         m_dataMgmt->Commit(m_buffer);
      }

      // The file is done loading indicate 100% progress and notify the application
      // that the parsing is complete.
      emit( setCurrentProgress(nProgressIncrements) );
      emit( fileDoneStatus(true, m_sFilename) );
   }

   void CsvParser::stopParse()
   {
      m_mutex.lock();
      m_bStop = true;
      m_mutex.unlock();
   }


   bool CsvParser::ExtractTokens(const QString& line, QStringList& tokens)
   {
      QString sData;
      bool bInQuotes = false;
      tokens.clear();
      for( int i = 0; i < line.size(); ++i )
      {
         QChar c = line.at(i);
         if( c == '\"' )
         {
            bInQuotes = !bInQuotes;
         }
         else if( !bInQuotes && c == m_cDelim )
         {
            tokens.append(sData);
            sData.clear();
         }
         else
         {
            // This if statement trims whitespace from the leading edge of the token.
            if( !sData.isEmpty() || c != ' ' )
            {
               sData.append(c);
            }
         }
      }

      // Make sure we got the last column.
      if( !sData.isEmpty() )
      {
         tokens.append(sData);
      }
      return true;
   }
};
