// Is this class supposed to hold a buffer of the frames needed to show?

#ifndef TIMESLIDER_H
#define TIMESLIDER_H

#include <map>

#include <QWidget>
#include <QSlider>
#include <QHBoxLayout>

#include "LinkLabel.hpp"

class TimeSlider : public QWidget
{
Q_OBJECT
public:
    TimeSlider();

    // Do we need a time class to return?
    double currentTime();

public slots:
    void onPlayToggled(bool value);
    void onNextClicked();
    void onPrevClicked();
    void onFirstClicked();
    void onLastClicked();
    void onIndexChanged(int);

private slots:
    void moveTimeTo(double time);       // Might need changed if we make a time class
    void advanceTime(int steps);
    void incrementTime();

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

#endif // TIMESLIDER_H
