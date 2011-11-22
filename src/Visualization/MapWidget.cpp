#include "mapwidget.h"

MapArea::MapArea(QWidget *parent) :
    QGraphicsWebView()
{

}

MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent)
{
    setMaximumSize(533,533);
    _scene = new QGraphicsScene(this);

    // Add the map the the scene
    _mapArea = new MapArea();
    _mapArea->setMaximumSize(533,533);
    //_mapArea->setResizesToContents(true);  // For some reason this looks weird

    // Get the map and put it in the MapArea
    updateMap();

    _scene->addItem(_mapArea);
}

void MapWidget::setMapView(QGraphicsView* view)
{
    _view = view;
    _view->setScene(_scene);
    _view->setMaximumSize(533,533);
}

void MapWidget::updateMap()
{
    // Resize the map area when we change the view to fit the widget
    _mapArea->resize(size());

    // Get the image from Google Maps by constructing a url with current sizes
    QString url("http://maps.google.com/maps/api/staticmap?size=");
    url += QString::number(size().width());
    url += "x";
    url += QString::number(size().height());
    url += "&center=30.32608,-87.39843&visible=30.29182,-87.49922|30.35468,-87.31792&maptype=hybrid&sensor=false";
    //std::cerr << url.toStdString() << '\n';
    _mapArea->setUrl(QUrl(url));
}

void MapWidget::resizeEvent(QResizeEvent* event)
{
    updateMap();
    _mapArea->resize(size());
    _view->resize(size());
}
