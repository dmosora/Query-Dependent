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


#include <iostream>

#include "TimeSlider.h"
#include "LinkLabel.hpp"
#include "DataTypes.h"
#include "DataMgmt.h"

// This will help us draw and keep track of the plane icon.
// NOTE: This should be a QGraphicsItem for control with the GraphicsScene
class AircraftOverlay : public QGraphicsItem
{
public:
    AircraftOverlay(QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);

private:
    // Coords of the center of the graphic (translate in drawing by shifting)
    int _posX;
    int _posY;

    QPixmap* _image;
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
    void drawFlightPath(QString flight_id);
    void drawPlane(QString flight_id);

    QPoint gpsToPixels(double lat, double lon);
    QPoint pixelsToGps(int x, int y);

signals:

public slots:
    void onActiveFlightChanged(QString);
    void onTimeChanged(int);

protected:
    virtual void resizeEvent(QResizeEvent* event);

private:
    // Data update method
    void getNewAttributes();

    // View components
    QGraphicsScene* _scene;
    QGraphicsView*  _view;          // The view in which this map is contained in the MDI
    MapArea*        _mapArea;
    TimeSlider*     _slider;        // For access to current time

    // Background map
    QPixmap*        _picMap;        // For static map

    // Drawing related things
    double          _latStart;
    double          _lonStart;
    double          _latEnd;
    double          _lonEnd;

    // Data
    int                 _currentIndex;
    QStringList         _flights;
    QString             _activeFlight;
    QList<QStringList>  _loadedAttributes;
    Data::DataMgmt*      m_dataMgmt;
};

#endif // MAPWIDGET_H
