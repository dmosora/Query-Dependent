// Written by David Sheets

#include <iostream>

#include <QPixmap>
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QVariant>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "CsvParser.h"


CsvParser::CsvParser(QObject* parent)
	: m_sTableName("DataTable")
	, m_sConnectionName("")
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
      const QString& sTableName,
      const QString& sConnection )
{
	// When a new file is received there is a new definition which means a new
	// database configuration.  Clear the definition list to ensure there aren't
	// any stagnant information.
	m_mutex.lock();
	m_sFilename = sFilename;
	m_sTableName = sTableName;
	m_sConnectionName = sConnection;
	m_paramDef.clear();
	m_mutex.unlock();
}

const int nProgressIncrements = 100;
void CsvParser::run()
{
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
      emit( setRange(0, nProgressIncrements) );
      emit( setValue(0) );

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
					if( !ProcessHeader( hdrTokens, tokens ) )
					{
						std::cerr << "Error extracting header information." << std::endl;
						emit( SIGNAL(fileDone(false)) );
						return;
					}
				}
			}
		}
		
		// Read in the rest of the file.
		QSqlDatabase db = QSqlDatabase::database( m_sConnectionName );
		db.transaction();
		while( !line.isNull() )
		{
			ExtractTokens( line, tokens );
			ProcessData( tokens );
			line = stream.readLine();
         llFilePos += line.length();
         
         if( (llFilePos - (llFileInc*nLastProg)) > llFileInc )
         {
            // Calculate a progress increment
            ++nLastProg;
            emit( setValue(nLastProg) );

				db.commit();
				db.transaction();
			}
		}
		db.commit();
	}
	
   // The file is done loading indicate 100% progress and notify the application
   // that the parsing is complete.
   emit( setValue(nProgressIncrements) );
	emit( SIGNAL(fileDone(true, m_sTableName)) );
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

bool CsvParser::ProcessHeader( const QStringList& hdr, const QStringList& data )
{
#if 1
	std::cout << "Header row: ";
	int i = 0;
	for( ; i < hdr.size()-1; ++i )
	{
		std::cout << qPrintable(hdr.at(i)) << " | ";
	}
	
	std::cout << qPrintable(hdr.at(i)) << std::endl;
#endif

	if( hdr.size() != data.size() )
	{
		return false;
	}

	// If the database contains the indicated table already the procedure is
	// to delete it and recreate it from the data in the file being parsed.
	QSqlDatabase db = QSqlDatabase::database( m_sConnectionName );
	db.transaction();
	QSqlQuery q(db);
	QString sQuery = "DROP TABLE IF EXISTS ";
	sQuery.append( m_sTableName );
	if( !q.exec(sQuery) )
	{
		QSqlError err = q.lastError();
		std::cerr << "Error creating table " << qPrintable(m_sTableName) << std::endl;
		std::cerr << "   Error message: " << qPrintable( err.text() ) << std::endl;
	}
	
	// Construct the table based on information extracted from the CSV file.
	Definition def;
	sQuery = "CREATE TABLE " + m_sTableName + "(ID INTEGER PRIMARY KEY AUTOINCREMENT,";
	bool bSuccess = true;
	for( int i = 0; i < data.size(); ++i )
	{
		QString  column(hdr.at(i));
		QVariant dim(data.at(i));

		def.sParamName = column;
		def.sParamNameComp = column.replace(" ", "");
		def.sParamNameComp = column.replace(",", "");
		def.sParamNameComp = column.replace("/", "");
		def.sParamNameComp = column.replace("-", "");
		def.sParamNameComp = column.replace("*", "");
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
		m_paramDef.push_back(def);
	}
	sQuery.replace(sQuery.length()-1, 1, ')');

	if( !q.exec(sQuery) )
	{
		std::cerr << "Failed to create table " << qPrintable(m_sTableName) << " for database " << qPrintable(m_sConnectionName) << std::endl;
		std::cerr << "   Query: " << qPrintable(sQuery) << std::endl;
		std::cerr << "   Error Message: " << qPrintable(q.lastError().text()) << std::endl;
		db.rollback();
		return false;
	}

	db.commit();
	return true;
}

void CsvParser::ProcessData( const QStringList& data )
{

	QSqlDatabase db = QSqlDatabase::database( m_sConnectionName );
	QSqlQuery q(db);
	QString sQueryColumns = "INSERT INTO " + m_sTableName + "(";
	QString sQueryValues  = " VALUES(";

	QString value;
	bool bSuccess = true;
	for( int i = 0; i < data.size(); ++i )
	{
		if( m_paramDef.at(i).eParamType == ParamType_String )
		{
			value = "\"" + data.at(i) + "\"";
		}
		else
		{
			value = data.at(i);
			
			// Make sure that if the value isn't a number that it results in a null.
			QVariant dim(data.at(i));
			dim.toFloat(&bSuccess);
			if( !bSuccess )
			{
				value = "";
			}
		}

		if( !value.isEmpty() )
		{
			sQueryColumns.append(m_paramDef.at(i).sParamNameComp);
			sQueryColumns.append(",");
			sQueryValues.append(value);
			sQueryValues.append(",");
		}
	}
	sQueryColumns.replace(sQueryColumns.length()-1, 1, ')');
	sQueryValues.replace (sQueryValues.length()-1, 1, ')');
	QString sQuery = sQueryColumns + sQueryValues;

	if( !q.exec(sQuery) )
	{
		std::cerr << "Failed to insert data into table " << qPrintable(m_sTableName)
                << " for database " << qPrintable(m_sConnectionName) << std::endl;
		std::cerr << "   Query: " << qPrintable(sQuery) << std::endl;
		std::cerr << "   Error Message: " << qPrintable(q.lastError().text()) << std::endl;
	}
}
