#include "mapwidget.h"

MapArea::MapArea(QWidget *parent) :
    QGraphicsWebView()
{

}

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
    _scene = new QGraphicsScene(this);

    _mapArea = new MapArea();
    _mapArea->setUrl(QUrl(QString("http://maps.google.com/maps/api/staticmap?size=600x600&center=30.32608,-87.39843&visible=30.29182,-87.49922|30.35468,-87.31792&maptype=hybrid&sensor=false")));
    _scene->addItem(_mapArea);

    _view = new QGraphicsView(_scene);
    _view->show();
}

MapWidget::MapWidget(QGraphicsView* view, QWidget* parent) {
    _scene = new QGraphicsScene(this);

    _mapArea = new MapArea();
    _mapArea->setUrl(QUrl(QString("http://maps.google.com/maps/api/staticmap?size=600x600&center=30.32608,-87.39843&visible=30.29182,-87.49922|30.35468,-87.31792&maptype=hybrid&sensor=false")));
    _scene->addItem(_mapArea);

    _view = view;
    _view->setScene(_scene);
}
