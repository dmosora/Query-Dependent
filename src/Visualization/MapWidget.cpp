#include "MapWidget.h"

AircraftOverlay::AircraftOverlay(QGraphicsItem* parent, QGraphicsScene* scene)
    : QGraphicsItem(parent, scene)
{
    _image = new QPixmap(":/Visualization/images/plane_right.png");
}

MapArea::MapArea(QWidget *parent) :
    QGraphicsWebView()
{

}

MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent)
{
    _latStart = 30.412558;
    _lonStart = -87.517914;
    _latEnd   = 30.232374;
    _lonEnd   = -87.276215;

    _scene = new QGraphicsScene(this);

    // Add the map the the scene
    _picMap = new QPixmap(QString(":/Visualization/images/map.png"));

    QGraphicsPixmapItem* item = _scene->addPixmap(*_picMap);
    item->setPos(0,0);
}

void MapWidget::setMapView(QGraphicsView* view)
{
    _view = view;
    _view->setScene(_scene);
    _view->setMaximumSize(589,514);
    _view->setMinimumSize(_view->maximumSize());
}

/// Only needed for dynamic maps with URL (or javascript retrieval)
void MapWidget::updateMap()
{ }

void MapWidget::resizeEvent(QResizeEvent* event)
{
    //updateMap();
    //_mapArea->resize(size());
    _view->resize(size());
}

void MapWidget::getFlightData()
{
    /*// Get the data we need to draw the path
    // Should this get all data up to the point on the slider?
    // Or just all data at once then go through it.
    std::vector<float> data;

    // This will help with multiple flights
    // Data::getLoadedFlights(QStringList&)

    const Data::EventDatabase& evtDb = m_dataMgmt.GetEventData();
    Data::EventDatabaseIterator iDb(evtDb);
    while( iDb.hasNext() )
    {
       iDb.next();
       data.clear();
       Data::EventDataIterator iList(iDb.value());
       while( iList.hasNext() )
       {
          data.push_back( iList.next()._valueNormal.toFloat() );
       }
       //event_glyph->CreatePointSet(data, iDb.key().toStdString());
    }*/
}



/// Old Code
/*MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent)
{
    /// Pixmap map code
    _latStart = 30.412558;
    _lonStart = -87.517914;
    _latEnd   = 30.232374;
    _lonEnd   = -87.276215;

    _scene = new QGraphicsScene(this);

    // Add the map the the scene
    _picMap = new QPixmap(QString(":/Visualization/images/map.png"));

    QGraphicsPixmapItem* item = _scene->addPixmap(*_picMap);
    item->setPos(0,0);

    /// Dynamic map code
    // This needs to be changed to use a static map with defined coordinates
    // That way we can rely on the positioning of our visual elements.
/*
    setMaximumSize(533,533);
    _scene = new QGraphicsScene(this);

    // Add the map the the scene
    _mapArea = new MapArea();
    _mapArea->setMaximumSize(533,533);
    //_mapArea->setResizesToContents(true);  // For some reason this looks weird

    // Get the map and put it in the MapArea
    updateMap();

    _scene->addItem(_mapArea);
    *//*
}

/// Only needed for dynamic maps with URL (or javascript retrieval)
void MapWidget::updateMap()
{
    // Resize the map area when we change the view to fit the widget
    /*_mapArea->resize(size());

    // Get the image from Google Maps by constructing a url with current sizes
    QString url("http://maps.google.com/maps/api/staticmap?size=");
    url += QString::number(size().width());
    url += "x";
    url += QString::number(size().height());
    url += "&center=30.32608,-87.39843&visible=30.29182,-87.49922|30.35468,-87.31792&maptype=hybrid&sensor=false";
    //std::cerr << url.toStdString() << '\n';
    _mapArea->setUrl(QUrl(url));
    */
//}
