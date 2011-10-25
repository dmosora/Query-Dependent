#include "mapwidget.h"

MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent)
{
    // Make a test query so we can start gathering this data
    // TODO: This is code copied from the query in Chart
    //       It needs work when the database access is fixed
    /*QSqlDatabase db = QSqlDatabase::database( m_sConnectionName );
    QSqlQuery q(db);
    QString sQuery = "SELECT * from " + m_sDataName;
    if( !q.exec(sQuery) )
    {
            QSqlError err = q.lastError();
            std::cerr << "Error querying table " << qPrintable(m_sDataName) << std::endl;
            std::cerr << "   Error message: " << qPrintable( err.text() ) << std::endl;
    }*/
}
