#include "TimeSlider.h"

// These all need modified to refer to the time instead of the version
// Speed will need adjusted

TimeSlider::TimeSlider(QWidget* parent) : QToolBar(parent)
{
    _currentTime = 0.0;

    // Set num equal to the first time field and add it so it will wrap around
    // to an actual time value.
    double num;
    _currentTime += num;

    // Initialize all the other attributes and set up the images
    _slider = new QSlider(Qt::Horizontal,this);
    _slider->setRange(0,10);    // Change this to amount of data points

    // Make a little media bar that interoperates with the time slider.
    _first = new LinkLabel(QPixmap(":/Visualization/images/first_up.png"), QPixmap(":/Visualization/images/first_down.png"), QString("First Version"), this);
    _last = new LinkLabel(QPixmap(":/Visualization/images/last_up.png"), QPixmap(":/Visualization/images/last_down.png"), QString("Latest Version"), this);
    _play = new LinkLabel(QPixmap(":/Visualization/images/play_up.png"), QPixmap(":/Visualization/images/play_down.png"), QString("Play"), this);
    _play->setCheckable(true);

    _first->setEnabled(true);
    _last->setEnabled(true);
    _play->setEnabled(true);
    _play->setChecked(false);   // Always start in non-play mode...

    _first->scaleToHeight(30);
    _play->scaleToHeight(30);
    _last->scaleToHeight(30);

    // Layout for Playback options
    _loop = new QCheckBox(tr("Loop"), this);
    _delay = new LinkLabel(tr("Delay (secs):"));
    _delay->setEnabled(false);
    _delay->setStyleSheet(QString("QLabel { color : black; }"));
    _delay->newToolTip(tr("Time in seconds between frames on playback."));
    _interval = new QDoubleSpinBox();
    _interval->setRange(0.25, 2.0);
    _interval->setDecimals(2);
    _interval->setSingleStep(0.25);
    _interval->setValue(2.0);

    addWidget(_first);
    addWidget(_play);
    addWidget(_last);
    addWidget(_slider);
    addWidget(_delay);
    addWidget(_interval);
    addWidget(_loop);

    _first->show();
    _play->show();
    _last->show();

    // Create the timer for media-style play.
    _timer = new QTimer(this);
    _timer->setInterval((_interval->value()) * 1000);
    _timer->setSingleShot(false);

    // Connect signals, etc.
    connect(_slider, SIGNAL(valueChanged(int)), SLOT(onTimeChanged()));
    connect(_play, SIGNAL(toggled(bool)), SLOT(onPlayToggled(bool)));
    connect(_first, SIGNAL(clicked()), SLOT(onFirstClicked()));
    connect(_last, SIGNAL(clicked()), SLOT(onLastClicked()));
    connect(_timer, SIGNAL(timeout()), SLOT(onNextClicked()));
    connect(_interval, SIGNAL(valueChanged(double)), SLOT(onIntervalChanged(double)));
}

/** Called when the time changes. */
void TimeSlider::onTimeChanged()
{
    // When time is updated, emit a signal to update other views as needed
    // The signal doesn't pass the value, but signals to the view that it needs updated
    // The view then grabs the current values from the view.
    emit timeChanged();
}

void TimeSlider::onPlayToggled(bool value)
{
    // This ensures the picture is changed if triggered from elsewhere.
    /*_play->setState(!value);

    if(value) {
        _timer->start();
        // If it's at the end, move it to the beginning
        if(spec->version() == spec->versions() - 1) {
            _slider->setValue(0);
        }
        //_play->newImage(QPixmap(":/Visualization/images/pause_up.png"), QPixmap(":/Visualization/images/pause_down.png"), tr("Pause"));
        _next->setEnabled(false);
        _prev->setEnabled(false);
    } else {
        _timer->stop();
        _play->newImage(QPixmap(":/Visualization/images/play_up.png"), QPixmap(":/Visualization/images/play_down.png"), tr("Play"));
        _next->setEnabled(true);
        _prev->setEnabled(true);
    }*/
}

void TimeSlider::onNextClicked()
{
    /*if(spec->version() == spec->versions() - 1) {
        // If we're on the last frame and looping is enabled, move to the
        // first frame. Otherwise, do nothing.
        if(_loop->isChecked()) {
            _slider->setValue(0);
        } else {
            // Otherwise, if we're playing we stop on first version.
            if(_play->isChecked()) {
                _play->setChecked(false);
            }
        }
    } else {
        _slider->triggerAction(QSlider::SliderSingleStepAdd);
    }
    emit attributeUpdated(spec);
    */
}

void TimeSlider::onPrevClicked()
{
    /*// Current time instead of version
    //AttributeSpec* spec = currentAttribute();
    if(spec->version() == 0) {
        // If we're on the first frame and looping is enabled, move to the
        // last frame. Otherwise, do nothing.
        if(_loop->isChecked()) {
            _slider->setValue(_slider->maximum());
        }
    } else {
        _slider->triggerAction(QSlider::SliderSingleStepSub);
    }
    emit attributeUpdated(spec);
    */
}

void TimeSlider::onFirstClicked()
{
/*    // Might need to fetch first time instead of this
    // The question is absolute time vs. relative time
    _currentTime->setHMS(0,0,0);
    emit attributeUpdated(spec);
*/
}

void TimeSlider::onLastClicked()
{
/*    // Change this to last time
    _slider->setValue(spec->versions() - 1);
    emit attributeUpdated(spec);
*/
}

void TimeSlider::onIntervalChanged(double val)
{ /*_timer->setInterval(val * 1000); */}

// Advance the time slider a certain amount of steps and wraparound if able
void TimeSlider::advanceTime(int steps) {
    for(int i = 0;i < steps; i++) {
        if(_slider->value() == _slider->maximum() && _loop->isChecked())
            _slider->setValue(0);
        incrementTime();
    }
}

void TimeSlider::incrementTime()
{
    _slider->triggerAction(QSlider::SliderSingleStepAdd);
}
