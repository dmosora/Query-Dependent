#include "MapWidget.h"

AircraftOverlay::AircraftOverlay(QGraphicsItem* parent, QGraphicsScene* scene)
    : QGraphicsItem(parent, scene)
{
    _image = new QPixmap(":/Visualization/images/plane_right.png");
    _image->scaledToWidth(50);
}

void AircraftOverlay::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
    // Convert coordinates to pixels and shift them for the drawPixmap operation
    QPoint point = gpsToPixels(_lat.toString().toDouble(), _lon.toString().toDouble());

    point.setX(point.x() - 25);
    point.setY(point.y() - 20);

    // Debug
    std::cerr << "Drawing plane at " << point.x() << " , " << point.y() << std::endl;

    painter->drawPixmap(point, *_image);
}

QPoint AircraftOverlay::gpsToPixels(double lat, double lon) {
    QPoint point;

    double latRange = _latStart - _latEnd;
    if(latRange < 0) latRange = -latRange;

    double lonRange = _lonStart - _lonEnd;
    if(lonRange < 0) lonRange = -lonRange;

    point.setX((lat/latRange) * 587);
    point.setY((lon/lonRange) * 511);
}

//***********************************

FlightPath::FlightPath(QGraphicsItem *parent) : QGraphicsItem(parent)
{ }

void FlightPath::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
    // Implement the drawing of the path itself here
}

//***********************************

/// Not using MapArea right now
MapArea::MapArea(QWidget *parent) :
    QGraphicsWebView()
{

}

//***********************************

MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent), _attributes(), _flights(), _activeFlight(), _loadedFlightsData()
{
    /*_latStart = 30.412558;
    _lonStart = -87.517914;
    _latEnd   = 30.232374;
    _lonEnd   = -87.276215;
    */
    _plane = 0;

    _activeFlightIdx = 0;

    _scene = new QGraphicsScene(this);

    // Add the map the the scene
    _picMap = new QPixmap(QString(":/Visualization/images/map.png"));

    // Set up the attribute names we need
    _attributes.push_back(QString("Latitude"));
    _attributes.push_back(QString("Longitude"));

    QGraphicsPixmapItem* item = _scene->addPixmap(*_picMap);
    item->setPos(0,0);
}

void MapWidget::setMapView(QGraphicsView* view)
{
    _view = view;
    _view->setScene(_scene);
    _view->setMaximumSize(589,514);
    _view->setMinimumSize(_view->maximumSize());

    connect(_scene, SIGNAL(changed(QList<QRectF>)),_view,SLOT(updateScene(QList<QRectF>)));
}

void MapWidget::setTimeSlider(TimeSlider* slider)
{
    _slider = slider;
}

void MapWidget::updateMap()
{
    // Clear the drawing space
    // Remove the Aircraft Overlay object
    if(_plane) _scene->removeItem(_plane);

    // Remove the  path objects
    for(int i = 0;i < _paths.size(); i++) {
        if(_paths[i]) _scene->removeItem(_paths[i]);
    }

    // Update our data
    getNewAttributes();

    // Draw and position the new paths and plane
    drawMapVis();
    _view->update();
}

// Invoke the drawing of the flight path for flights loaded
// and determine the currently being viewed flight
void MapWidget::drawMapVis()
{
    // Zero out old paths and plane cause they have been removed from the view
    if(_plane) delete _plane; _plane = 0;
    _paths.clear();

    // Draw flight paths here
    for(int i = 0;i < _flights.size(); i++) {
        drawFlightPath(_flights[i], i);
    }

    // Draw active plane here
    drawPlane(_activeFlight, _activeFlightIdx);
}

// Draw the data up to the current time in dots
void MapWidget::drawFlightPath(QString flight_id, int index)
{
    // Create a new FlightPath
    FlightPath* path = new FlightPath();

    // Set its data and index
    path->setLocationData(&_loadedFlightsData.at(index)._params.at(0),&_loadedFlightsData.at(index)._params.at(1));

    // Set its location in the view
    path->setPos(0,0);

    // Make it draw
    _scene->addItem(path);

    // Save it to be removed later and redrawn
    _paths.push_back(path);
}

// Draw the plane icon at the current time
void MapWidget::drawPlane(QString flight_id, int index)
{
    // Create a new AircraftOverlay
    _plane = new AircraftOverlay();

    // Set its coordinates
    if(_currentIndex <= _loadedFlightsData.at(index)._params.at(1)._dataVector.size())
        _plane->setLocationData(&_loadedFlightsData.at(index)._params.at(_currentIndex)._dataVector.at(0),
                                &_loadedFlightsData.at(index)._params.at(_currentIndex)._dataVector.at(1));

    // Make it draw
    _scene->addItem(_plane);
    _plane->show();
}

void MapWidget::onActiveFlightChanged(QString flight)
{
    _activeFlight = flight;

    // Clear the drawing space and draw a new plane icon
    updateMap();
}

void MapWidget::onActiveFlightIndexChanged(int idx)
{
    _activeFlightIdx = idx;
}

void MapWidget::onTimeChanged(int idx)
{
    // Get the new time from the _slider
    // Update the map
    _currentIndex = idx;
    updateMap();
}

// Redo this if you have time to get only one flight's lat/lon
void MapWidget::getNewAttributes()
{
    // Get the currently loaded flight names
    m_dataMgmt->GetLoadedFlights(_flights);

    // Push the attributes we need
    if(_loadedFlightsData.size() < _flights.size()) {
        Data::Buffer buffer;
        m_dataMgmt->GetDataAttributes(_flights.back(), _attributes, buffer);

        _loadedFlightsData.push_back(buffer);
    }
}

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
