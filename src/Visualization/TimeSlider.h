// Is this class supposed to hold a buffer of the frames needed to show?

#ifndef TIMESLIDER_H
#define TIMESLIDER_H

#include <iostream>
#include <map>

#include <QWidget>
#include <QSlider>
#include <QHBoxLayout>
#include <QTime>
#include <QTimer>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QToolBar>

#include "LinkLabel.hpp"

class TimeSlider : public QToolBar
{
Q_OBJECT
public:
    TimeSlider(QWidget* parent = 0);

    // Do we need a time class to return?
    double currentTime();

public slots:
    void onTimeChanged();
    void onPlayToggled(bool value);
    void onNextClicked();
    void onPrevClicked();
    void onFirstClicked();
    void onLastClicked();
    void onIntervalChanged(double);

signals:
    void timeChanged();

private slots:
    //void moveTimeTo(double time);       // Might need changed if we make a time class
    void advanceTime(int steps);
    void incrementTime();

private:
    QSlider*        _slider;
    double          _currentTime;
    int             _currentTimeIndex;  // For the slider's index
    QHBoxLayout*    _mediaBar;
    LinkLabel*      _play;
    LinkLabel*      _pause;
    LinkLabel*      _first;
    LinkLabel*      _last;

    // Control widgets
    QCheckBox*      _loop;
    LinkLabel*      _delay;
    QDoubleSpinBox* _interval;

    QTimer*         _timer;
};

#endif // TIMESLIDER_H
