#ifndef SLIDER_H
#define SLIDER_H

#include <map>

#include <QWidget>
#include <QSlider>
#include <QHBoxLayout>

#include "LinkLabel.hpp"

class CodeFile;
class AttributeSpec;
class LinkLabel;

class TimeSlider
{
public:
    Slider();

private:
    QSlider*        _slider;
    QHBoxLayout*    _mediaBar;
    LinkLabel*      _play;
    LinkLabel*      _pause;
    LinkLabel*      _next;
    LinkLabel*      _prev;
    LinkLabel*      _first;
    LinkLabel*      _last;
};

#endif // SLIDER_H
