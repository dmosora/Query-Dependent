#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QtWebkit/QGraphicsWebView>

// This will help us draw and keep track of the plane icon.
class AircraftOverlay
{
public:
    AircraftOverlay() {}
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

signals:

public slots:

private:
    MapArea* _mapArea;
};

#endif // MAPWIDGET_H
