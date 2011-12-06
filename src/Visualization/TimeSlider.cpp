#include "TimeSlider.h"

// These all need modified to refer to the time instead of the version
// Speed will need adjusted

TimeSlider::TimeSlider(QWidget* parent) : QToolBar(parent)
{
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
    _delay = new LinkLabel(tr("Delay (ms):"));
    _delay->setEnabled(false);
    _delay->setStyleSheet(QString("QLabel { color : black; }"));
    _delay->newToolTip(tr("Time in milliseconds between frames on playback."));
    _interval = new QDoubleSpinBox();
    _interval->setRange(1, 20);
    _interval->setDecimals(0);
    _interval->setSingleStep(1);
    _interval->setValue(2);

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
    _timer->setInterval((_interval->value()));
    _timer->setSingleShot(false);

    // Set max at 0 to be set later by the Visualization object
    _slider->setMinimum(0);
    _slider->setMaximum(0);
    _max = 0;

    // Connect signals, etc.
    connect(_slider, SIGNAL(valueChanged(int)), SLOT(onTimeChanged()));
    connect(_play, SIGNAL(toggled(bool)), SLOT(onPlayToggled(bool)));
    connect(_first, SIGNAL(clicked()), SLOT(onFirstClicked()));
    connect(_last, SIGNAL(clicked()), SLOT(onLastClicked()));
    connect(_timer, SIGNAL(timeout()), SLOT(onNextClicked()));
    connect(_interval, SIGNAL(valueChanged(double)), SLOT(onIntervalChanged(double)));
}

void TimeSlider::setNewMax(int max)
{
     if(max > _max) {
        _max = max;
        _slider->setMaximum(max);
        std::cerr << "New max slider set at " << _max << std::endl;
     }
}

/** Called when the time changes. */
void TimeSlider::onTimeChanged()
{
    // When time is updated, emit a signal to update other views as needed
    // The signal doesn't pass the value, but signals to the view that it needs updated
    // The view then grabs the current values from the view.
    emit timeChanged(_slider->value());
}

void TimeSlider::onPlayToggled(bool value)
{
    // This ensures the picture is changed if triggered from elsewhere.
    _play->setState(!value);

    if(value) {
        _timer->start();
        // If it's at the end, move it to the beginning
        if(_slider->value() == _slider->maximum()) {
            _slider->setValue(0);
        }
        //_play->newImage(QPixmap(":/Visualization/images/pause_up.png"), QPixmap(":/Visualization/images/pause_down.png"), tr("Pause"));
    } else {
        _timer->stop();
        _play->newImage(QPixmap(":/Visualization/images/play_up.png"), QPixmap(":/Visualization/images/play_down.png"), tr("Play"));
        _play->scaleToHeight(30);
    }
}

void TimeSlider::onNextClicked()
{
    if(_slider->value() == _slider->maximum()) {
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
    emit timeChanged(_slider->value());
}

void TimeSlider::onPrevClicked()
{
    // Current time instead of version
    //AttributeSpec* spec = currentAttribute();
    if(_slider->value() == 0) {
        // If we're on the first frame and looping is enabled, move to the
        // last frame. Otherwise, do nothing.
        if(_loop->isChecked()) {
            _slider->setValue(_slider->maximum());
        }
    } else {
        _slider->triggerAction(QSlider::SliderSingleStepSub);
    }
    emit timeChanged(_slider->value());
}

void TimeSlider::onFirstClicked()
{
    _slider->setValue(0);
    emit timeChanged(_slider->value());
}

void TimeSlider::onLastClicked()
{
    _slider->setValue(_slider->maximum());
    emit timeChanged(_slider->value());
}

void TimeSlider::onIntervalChanged(double val)
{ _timer->setInterval(val); }

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
