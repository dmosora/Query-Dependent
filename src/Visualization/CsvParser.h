// Written by David Sheets

#ifndef CSVPARSER_H
#define CSVPARSER_H


#include <QThread>
#include <QMutex>

enum ParamType
{
	ParamType_String,
	ParamType_Numeric
};

class Definition
{
public:
	ParamType eParamType;
	QString   sParamName;
	QString   sParamNameComp;
};
//Q_DECLARE_METATYPE(Definition);
typedef QList<Definition> ParamDef;

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
         const QString& sTableName,
         const QString& sConnection );

   //! Returns a list of the columns in the header row
	QStringList GetHeaderRow();
	

public slots:
	void stopParse();

signals:
	void fileDone(bool bSuccess, QString sTableName);
	void postProgress(int percentage);

   // Progress signals.
   void setRange(int min, int max);
   void setValue(int value);

protected:
	void run();
	
   //! Pulls out the content of each field and places it as a single entry
   //! in the tokens.  This handles quoted strings as a single token.
	bool ExtractTokens( const QString& line, QStringList& tokens );

	bool ProcessHeader( const QStringList& hdr, const QStringList& data);
	void ProcessData( const QStringList& hdr );

private:
	ParamDef    m_paramDef;
	QMutex      m_mutex;
	QString     m_sFilename;
	QString     m_sTableName;
	QString     m_sConnectionName;
	QChar       m_cDelim;
	bool        m_bStop;
	bool        m_bHasHeader;
};

#endif // CSVPARSER_H
