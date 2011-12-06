#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QHBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QtWebkit/QGraphicsWebView>
#include <QUrl>

#include <cstdlib>
#include <iostream>

#include "TimeSlider.h"
#include "LinkLabel.hpp"
#include "DataTypes.h"
#include "DataMgmt.h"

// Global functions for conversions
/// QUICK IMPLEMENTATION, this needs to be moved for dynamic maps
const double _latStart = 30.412558;
const double _lonStart = -87.517914;
const double _latEnd   = 30.232374;
const double _lonEnd   = -87.276215;

//QPoint pixelsToGps(int x, int y);

// This will help us draw and keep track of the plane icon.
class AircraftOverlay : public QGraphicsItem
{
public:
    AircraftOverlay(QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);

    void setLocationData(double lat, double lon) { _lat = lat; _lon = lon; }
    QPoint gpsToPixels(double lat, double lon);

    QRectF boundingRect() const { return QRectF(0, 0, 50, 40); }

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

private:
    double _lat;
    double _lon;

    QPixmap* _image;
};

// Class to draw a path on
class FlightPath : public QGraphicsItem
{
public:
    FlightPath(QGraphicsItem *parent = 0);

    void setLocationData(const QList<Data::Point>* coords)
    { _coords = coords; }

    void setFinalIndex(int idx)
    {
        if(idx < _coords->size()) _finalIndex = idx;
        else _finalIndex = _coords->size() - 1;
    }

    QPoint gpsToPixels(double lat, double lon);

    QRectF boundingRect() const { return QRectF(0, 0, 587, 511); }

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

private:
    int                           _finalIndex;
    const QList<Data::Point>*     _coords;

};

// This will set up the background map area with a url request to Google Maps' API
// NOTE: This must be changed to a static image instead of the request.
//       Need to use Javascript to get the actual pixel coordinates.
/// Disabled until necessary for dynamic maps with Google's API
class MapArea : public QGraphicsWebView
{
public:
    explicit MapArea(QWidget *parent = 0);

public slots:

private:
    double          _latStart;
    double          _lonStart;
    double          _latEnd;
    double          _lonEnd;

    double          _url;
};

// The Architecture of this class is MapWidget controls the MapArea in a scene
// - The Scene will hold all the drawing things, like the trail and plane
class MapWidget : public QWidget
{
    Q_OBJECT
public:
    // Constructors
    explicit MapWidget(QWidget *parent = 0);
    MapWidget(QGraphicsView* view, QWidget *parent = 0);

    // Setters
    void setMapView(QGraphicsView* view);
    void setTimeSlider(TimeSlider* slider);
    void setDataMgmt(Data::DataMgmt* Mgmt) { m_dataMgmt = Mgmt; }
    void setActiveFlight(QString flight) { _activeFlight = flight; }

    // Drawing related methods
    void getFlightData();
    void updateMap();
    void drawMapVis();
    void drawFlightPath(QString flight_id, int index);
    void drawPlane(QString flight_id, int index);

    // Data update method
    void getNewAttributes();

signals:

public slots:
    void onActiveFlightChanged(QString);
    void onActiveFlightIndexChanged(int idx);
    void onTimeChanged(int);

protected:
    virtual void resizeEvent(QResizeEvent* event);

private:
    // View components
    QGraphicsScene*     _scene;
    QGraphicsView*      _view;          // The view in which this map is contained in the MDI
    MapArea*            _mapArea;
    TimeSlider*         _slider;        // For access to current time

    // Imagery
    QPixmap*            _picMap;        // For static map
    QList<FlightPath*>  _paths;
    //QWidget*            _pathSurface;
    AircraftOverlay*    _plane;

    // Drawing related things
    /// Disabled because they'll need to be here for dynamic maps
    /*double              _latStart;
    double              _lonStart;
    double              _latEnd;
    double              _lonEnd;
    */

    // Data
    int                   _currentIndex;
    QStringList           _attributes;
    QStringList           _flights;
    QString               _activeFlight;
    int                   _activeFlightIdx;
    QList<Data::Buffer>   _loadedFlightsData;          /// SPEED CAN BE IMPROVED HERE
    Data::DataMgmt*       m_dataMgmt;
};

#endif // MAPWIDGET_H
