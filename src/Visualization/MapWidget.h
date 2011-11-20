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

class MapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = 0);
    MapWidget(QGraphicsView* view, QWidget *parent = 0);

signals:

public slots:

private:
    QGraphicsScene* _scene;
    QGraphicsView* _view;
    MapArea* _mapArea;
};

#endif // MAPWIDGET_H
