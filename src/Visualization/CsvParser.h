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

#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <QThread>
#include <QMutex>


class DataMgmt;


class CsvParser : public QThread
{
	Q_OBJECT
public:
	CsvParser(QObject* parent = 0);
	~CsvParser();

	//! Sets the information needed to parse data from the CSV file.
	//! @param sFilename   Path and name of the CSV file 
	//! @param sTableName  Name of the data table
	//! @param sConnection Name of the data set
	void SetParseInformation( 
		const QString& sFilename,
		DataMgmt* dataMgmt,
		const QString& sTableName,
		const QString& sConnection );


public slots:
	//! Slot to handle an interrupt signal.  This will stop the file parsing.
	void stopParse();

signals:
	//! Signal indicating that the file parsing has completed.
	//! @param bSuccess  Indicates whether the file parsing was successful.
	//! @param sFileName Provides the file that was parsed.
	void fileDone(bool bSuccess, QString sFileName);
	void fileDone();


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
	void run();

	//! Pulls out the content of each field and places it as a single entry
	//! in the tokens.  This handles quoted strings as a single token.
	//! @param line    Input line of CSV values
	//! @param tokens  Processed list of values from the input line
	//! @retval  true  If the extraction was successful
	//! @retval  false Otherwise.
	bool ExtractTokens( const QString& line, QStringList& tokens );


private:
	QMutex      m_mutex;      //!< Mutex for thread safety
	DataMgmt*   m_dataMgmt;   //!< DataMgmt interface to store data
	QString     m_sFilename;  //!< Name of the file to be parsed.
	QChar       m_cDelim;     //!< Character matching the delimiter, e.g. ','
	bool        m_bStop;      //!< Flag indicating that parsing should stop.
	bool        m_bHasHeader; //!< Flag indicating whether the data has a header
};

#endif // CSVPARSER_H
