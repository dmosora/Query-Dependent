#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QHBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsWebView>
#include <QUrl>

#include <iostream>

#include "LinkLabel.hpp"

// This will help us draw and keep track of the plane icon.
// NOTE: This should be a QGraphicsItem for control with the GraphicsScene
class AircraftOverlay
{
public:
    AircraftOverlay() {}
private:
    // Coords of the top-left corner of the graphic
    int _posX;
    int _posY;
};

// This will set up the background map area with a url request to Google Maps' API
class MapArea : public QGraphicsWebView
{
public:
    explicit MapArea(QWidget *parent = 0);

public slots:

private:
    double _latStart;
    double _lonStart;
    double _latEnd;
    double _lonEnd;

    double _url;
};

// The Architecture of this class is MapWidget controls the MapArea in a scene
// - The Scene will hold all the drawing things, like the trail and plane
// TODO:
// - Needs logic to find coordinates in view and draw the plane
// - Buffer system to load all previous points and build a path from past data
// - More interaction with timeslider
class MapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = 0);
    MapWidget(QGraphicsView* view, QWidget *parent = 0);

    void setMapView(QGraphicsView* view);
    void updateMap();

signals:

public slots:

protected:
    virtual void resizeEvent(QResizeEvent* event);

private:
    QGraphicsScene* _scene;
    QGraphicsView* _view;
    MapArea* _mapArea;
};

#endif // MAPWIDGET_H
